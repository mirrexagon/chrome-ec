/* Copyright 2018 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Silergy SYV682x USB-C Power Path Controller */
#include "common.h"
#include "config.h"
#include "console.h"
#include "driver/ppc/syv682x.h"
#include "hooks.h"
#include "i2c.h"
#include "system.h"
#include "timer.h"
#include "usb_charge.h"
#include "usb_pd_tcpm.h"
#include "usbc_ppc.h"
#include "usb_pd.h"
#include "util.h"

#define SYV682X_FLAGS_SOURCE_ENABLED	BIT(0)
/* 0 -> CC1, 1 -> CC2 */
#define SYV682X_FLAGS_CC_POLARITY	BIT(1)
#define SYV682X_FLAGS_VBUS_PRESENT	BIT(2)
#define SYV682X_FLAGS_OCP		BIT(3)
#define SYV682X_FLAGS_OVP		BIT(4)
#define SYV682X_FLAGS_TSD		BIT(5)
#define SYV682X_FLAGS_RVS		BIT(6)
#define SYV682X_FLAGS_VCONN_OCP		BIT(7)

static uint32_t irq_pending; /* Bitmask of ports signaling an interrupt. */
static uint8_t flags[CONFIG_USB_PD_PORT_MAX_COUNT];

#define SYV682X_VBUS_DET_THRESH_MV		4000
/* Longest time that can be programmed in DSG_TIME field */
#define SYV682X_MAX_VBUS_DISCHARGE_TIME_MS	400
/* Delay between checks when polling the interrupt registers */
#define INTERRUPT_DELAY_MS 10

#define CPRINTS(format, args...) cprints(CC_USBPD, format, ## args)

static int read_reg(uint8_t port, int reg, int *regval)
{
	return i2c_read8(ppc_chips[port].i2c_port,
			 ppc_chips[port].i2c_addr_flags,
			 reg,
			 regval);
}

/*
 * During channel transition or discharge, the SYV682A silently ignores I2C
 * writes. Poll the BUSY bit until the SYV682A is ready.
 */
static int syv682x_wait_for_ready(int port)
{
	int regval;
	int rv;
	timestamp_t deadline;

	deadline.val = get_time().val
			+ (SYV682X_MAX_VBUS_DISCHARGE_TIME_MS * MSEC);

	do {
		rv = read_reg(port, SYV682X_CONTROL_3_REG, &regval);
		if (rv)
			return rv;

		if (!(regval & SYV682X_BUSY))
			break;

		if (timestamp_expired(deadline, NULL)) {
			CPRINTS("syv682x p%d: busy timeout", port);
			return EC_ERROR_TIMEOUT;
		}

		msleep(1);
	} while (1);

	return EC_SUCCESS;
}

static int write_reg(uint8_t port, int reg, int regval)
{
	int rv;

	rv = syv682x_wait_for_ready(port);
	if (rv)
		return rv;

	return i2c_write8(ppc_chips[port].i2c_port,
			  ppc_chips[port].i2c_addr_flags,
			  reg,
			  regval);
}

static int syv682x_is_sourcing_vbus(int port)
{
	return !!(flags[port] & SYV682X_FLAGS_SOURCE_ENABLED);
}

static int syv682x_discharge_vbus(int port, int enable)
{
	/*
	 * Smart discharge mode is enabled, nothing to do
	 */
	return EC_SUCCESS;
}

/* Filter interrupts with rising edge trigger */
static bool syv682x_interrupt_filter(int port, int regval, int regmask,
				     int flagmask)
{
	if (regval & regmask) {
		if (!(flags[port] & flagmask)) {
			flags[port] |= flagmask;
			return true;
		}
	} else {
		flags[port] &= ~flagmask;
	}
	return false;
}

/*
 * Two status registers can trigger the ALERT_L pin, STATUS and CONTROL_4
 * These registers are clear on read if the condition has been cleared.
 * The ALERT_L pin will not de-assert if the alert condition has not been
 * cleared. Since they are clear on read, we should check the alerts whenever we
 * read these registers to avoid race conditions.
 */
static void syv682x_handle_status_interrupt(int port, int regval)
{
	/* These conditions automatically turn off VBUS sourcing */
	if (regval &
	    (SYV682X_STATUS_OC_5V | SYV682X_STATUS_OVP | SYV682X_STATUS_TSD)) {
		flags[port] &= ~SYV682X_FLAGS_SOURCE_ENABLED;
	}

	/* Handle OC and thermal shutdown the same */
	if (syv682x_interrupt_filter(port, regval,
				     (SYV682X_STATUS_OC_HV |
				      SYV682X_STATUS_OC_5V |
				      SYV682X_STATUS_TSD),
				     SYV682X_FLAGS_OCP)) {
		pd_handle_overcurrent(port);
	}

	/* No PD handler for VBUS OVP/RVS events */
	if (syv682x_interrupt_filter(port, regval, SYV682X_STATUS_OVP,
				     SYV682X_FLAGS_OVP)) {
		CPRINTS("ppc p%d: VBUS OVP!", port);
	}
	if (syv682x_interrupt_filter(port, regval, SYV682X_STATUS_RVS,
				     SYV682X_FLAGS_RVS)) {
		CPRINTS("ppc p%d: VBUS Reverse Voltage!", port);
	}
}

static void syv682x_handle_control_4_interrupt(int port, int regval)
{
	if (syv682x_interrupt_filter(port, regval, SYV682X_CONTROL_4_VCONN_OCP,
				     SYV682X_FLAGS_VCONN_OCP)) {
		CPRINTS("ppc p%d: VCONN OC!", port);
	}

	/* This should never happen unless something really bad happened */
	if (regval & SYV682X_CONTROL_4_VBAT_OVP) {
		CPRINTS("ppc p%d: VBAT OVP!", port);
	}
}

static int syv682x_vbus_sink_enable(int port, int enable)
{
	int regval;
	int rv;

	if (!enable && syv682x_is_sourcing_vbus(port)) {
		/*
		 * We're currently a source, so nothing more to do
		 */
		return EC_SUCCESS;
	}

	/*
	 * For sink mode need to make sure high voltage power path is connected
	 * and sink mode is selected.
	 */
	rv = read_reg(port, SYV682X_CONTROL_1_REG, &regval);
	if (rv)
		return rv;

	if (enable) {
		/* Select high voltage path */
		regval |= SYV682X_CONTROL_1_CH_SEL;
		/* Select Sink mode and turn on the channel */
		regval &= ~(SYV682X_CONTROL_1_HV_DR |
			    SYV682X_CONTROL_1_PWR_ENB);
		flags[port] &= ~SYV682X_FLAGS_SOURCE_ENABLED;
	} else {
		/*
		 * No need to change the voltage path or channel direction. But,
		 * turn both paths off because we are currently a sink.
		 */
		regval |= SYV682X_CONTROL_1_PWR_ENB;
	}

	return write_reg(port, SYV682X_CONTROL_1_REG, regval);
}

#ifdef CONFIG_USB_PD_VBUS_DETECT_PPC
static int syv682x_is_vbus_present(int port)
{
	int val;
	int vbus = 0;

	if (read_reg(port, SYV682X_STATUS_REG, &val))
		return vbus;
	/*
	 * The status register interrupt bits are clear on read, check
	 * register value to see if there are interrupts to avoid race
	 * conditions with the interrupt handler
	 */
	syv682x_handle_status_interrupt(port, val);

	/*
	 * VBUS is considered present if VSafe5V is detected or neither VSafe5V
	 * or VSafe0V is detected, which implies VBUS > 5V.
	 */
	if ((val & SYV682X_STATUS_VSAFE_5V) ||
	    !(val & (SYV682X_STATUS_VSAFE_5V | SYV682X_STATUS_VSAFE_0V)))
		vbus = 1;
#ifdef CONFIG_USB_CHARGER
	if (!!(flags[port] & SYV682X_FLAGS_VBUS_PRESENT) != vbus)
		usb_charger_vbus_change(port, vbus);

	if (vbus)
		flags[port] |= SYV682X_FLAGS_VBUS_PRESENT;
	else
		flags[port] &= ~SYV682X_FLAGS_VBUS_PRESENT;
#endif

	return vbus;
}
#endif

static int syv682x_vbus_source_enable(int port, int enable)
{
	int regval;
	int rv;
	/*
	 * For source mode need to make sure 5V power path is connected
	 * and source mode is selected.
	 */
	rv = read_reg(port, SYV682X_CONTROL_1_REG, &regval);
	if (rv)
		return rv;

	if (enable) {
		/* Select 5V path and turn on channel */
		regval &= ~(SYV682X_CONTROL_1_CH_SEL |
			    SYV682X_CONTROL_1_PWR_ENB);
		/* Disable HV Sink path */
		regval |= SYV682X_CONTROL_1_HV_DR;
	} else if (flags[port] & SYV682X_FLAGS_SOURCE_ENABLED) {
		/*
		 * For the disable case, make sure that VBUS was being sourced
		 * prior to disabling the source path. Because the source/sink
		 * paths can't be independently disabled, and this function will
		 * get called as part of USB PD initialization, setting the
		 * PWR_ENB always can lead to broken dead battery behavior.
		 *
		 * No need to change the voltage path or channel direction. But,
		 * turn both paths off.
		 */
		regval |= SYV682X_CONTROL_1_PWR_ENB;
	}

	rv = write_reg(port, SYV682X_CONTROL_1_REG, regval);
	if (rv)
		return rv;

	if (enable)
		flags[port] |= SYV682X_FLAGS_SOURCE_ENABLED;
	else
		flags[port] &= ~SYV682X_FLAGS_SOURCE_ENABLED;

#if defined(CONFIG_USB_CHARGER) && defined(CONFIG_USB_PD_VBUS_DETECT_PPC)
	/*
	 * Since the VBUS state could be changing here, need to wake the
	 * USB_CHG_N task so that BC 1.2 detection will be triggered.
	 */
	usb_charger_vbus_change(port, enable);
#endif

	return EC_SUCCESS;
}

static int syv682x_set_vbus_source_current_limit(int port,
						 enum tcpc_rp_value rp)
{
	int rv;
	int limit;
	int regval;

	rv = read_reg(port, SYV682X_CONTROL_1_REG, &regval);
	if (rv)
		return rv;

	/* We need buffer room for all current values. */
	switch (rp) {
	case TYPEC_RP_3A0:
		limit = SYV682X_5V_ILIM_3_30;
		break;

	case TYPEC_RP_1A5:
		limit = SYV682X_5V_ILIM_1_75;
		break;

	case TYPEC_RP_USB:
	default:
		/* 1.25 A is lowest current limit setting for SVY682 */
		limit = SYV682X_5V_ILIM_1_25;
		break;
	};

	regval &= ~SYV682X_5V_ILIM_MASK;
	regval |= (limit << SYV682X_5V_ILIM_BIT_SHIFT);
	return write_reg(port, SYV682X_CONTROL_1_REG, regval);
}

#ifdef CONFIG_USBC_PPC_POLARITY
static int syv682x_set_polarity(int port, int polarity)
{
	/*
	 * The SYV682x does not explicitly set CC polarity. However, if VCONN is
	 * being used then the polarity is required to connect 5V to the correct
	 * CC line. So this function saves the CC polarity as a bit in the flags
	 * variable so VCONN is connected the correct CC line. The flag bit
	 * being set means polarity = CC2, the flag bit clear means
	 * polarity = CC1.
	 */
	if (polarity)
		flags[port] |= SYV682X_FLAGS_CC_POLARITY;
	else
		flags[port] &= ~SYV682X_FLAGS_CC_POLARITY;

	return EC_SUCCESS;
}
#endif

#ifdef CONFIG_USBC_PPC_VCONN
static int syv682x_set_vconn(int port, int enable)
{
	int regval;
	int rv;

	rv = read_reg(port, SYV682X_CONTROL_4_REG, &regval);
	if (rv)
		return rv;
	/*
	 * The control4 register interrupt bits are clear on read, check
	 * register value to see if there are interrupts to avoid race
	 * conditions with the interrupt handler
	 */
	syv682x_handle_control_4_interrupt(port, regval);

	if (enable)
		regval |= flags[port] & SYV682X_FLAGS_CC_POLARITY ?
			SYV682X_CONTROL_4_VCONN1 : SYV682X_CONTROL_4_VCONN2;
	else
		regval &= ~(SYV682X_CONTROL_4_VCONN2 |
			    SYV682X_CONTROL_4_VCONN1);

	return write_reg(port, SYV682X_CONTROL_4_REG, regval);
}
#endif

#ifdef CONFIG_CMD_PPC_DUMP
static int syv682x_dump(int port)
{
	int reg_addr;
	int data;
	int rv;
	const int i2c_port = ppc_chips[port].i2c_port;
	const int i2c_addr_flags = ppc_chips[port].i2c_addr_flags;

	for (reg_addr = SYV682X_STATUS_REG; reg_addr <= SYV682X_CONTROL_4_REG;
	     reg_addr++) {
		rv = i2c_read8(i2c_port, i2c_addr_flags, reg_addr, &data);
		if (rv)
			ccprintf("ppc_syv682[p%d]: Failed to read reg 0x%02x\n",
				 port, reg_addr);
		else
			ccprintf("ppc_syv682[p%d]: reg 0x%02x = 0x%02x\n",
				 port, reg_addr, data);
	}

	cflush();

	return EC_SUCCESS;
}
#endif /* defined(CONFIG_CMD_PPC_DUMP) */

static void syv682x_interrupt_delayed(int port, int delay);

static void syv682x_handle_interrupt(int port)
{
	int control4;
	int status;

	/* Both interrupt registers are clear on read */
	read_reg(port, SYV682X_CONTROL_4_REG, &control4);
	syv682x_handle_control_4_interrupt(port, control4);

	read_reg(port, SYV682X_STATUS_REG, &status);
	syv682x_handle_status_interrupt(port, status);

	/*
	 * Since ALERT_L is level-triggered, check the alert status and repeat
	 * until all interrupts are cleared. This will not spam indefinitely on
	 * OCP, but may on OVP, RVS, or TSD
	 */

	if (IS_ENABLED(CONFIG_USBC_PPC_DEDICATED_INT) &&
	    ppc_get_alert_status(port)) {
		syv682x_interrupt_delayed(port, INTERRUPT_DELAY_MS);
	} else {
		read_reg(port, SYV682X_CONTROL_4_REG, &control4);
		read_reg(port, SYV682X_STATUS_REG, &status);
		if (status & SYV682X_STATUS_INT_MASK ||
		    control4 & SYV682X_CONTROL_4_INT_MASK) {
			syv682x_interrupt_delayed(port, INTERRUPT_DELAY_MS);
		}
	}
}

static void syv682x_irq_deferred(void)
{
	int i;
	uint32_t pending = atomic_read_clear(&irq_pending);

	for (i = 0; i < board_get_usb_pd_port_count(); i++)
		if (BIT(i) & pending)
			syv682x_handle_interrupt(i);
}
DECLARE_DEFERRED(syv682x_irq_deferred);

static void syv682x_interrupt_delayed(int port, int delay)
{
	atomic_or(&irq_pending, BIT(port));
	hook_call_deferred(&syv682x_irq_deferred_data, delay * MSEC);
}

void syv682x_interrupt(int port)
{
	/* FRS timings require <15ms response to an FRS event */
	syv682x_interrupt_delayed(port, 0);
}

static bool syv682x_is_sink(uint8_t control_1)
{
	/*
	 * The SYV682 integrates power paths: 5V and HV (high voltage).
	 * The SYV682 can source either 5V or HV, but only sinks on the HV path.
	 *
	 * PD analyzer without a device connected confirms the SYV682 acts as
	 * a source under these conditions:
	 *	HV_DR && !CH_SEL:	source 5V
	 *	HV_DR && CH_SEL:	source 15V
	 *	!HV_DR && !CH_SEL:	source 5V
	 *
	 * The SYV682 is only a sink when !HV_DR && CH_SEL
	 */
	if (!(control_1 & SYV682X_CONTROL_1_PWR_ENB)
		&& !(control_1 & SYV682X_CONTROL_1_HV_DR)
		&& (control_1 & SYV682X_CONTROL_1_CH_SEL))
		return true;

	return false;
}

static int syv682x_init(int port)
{
	int rv;
	int regval;
	int status, control_1;
	enum tcpc_rp_value initial_current_limit;

	rv = read_reg(port, SYV682X_STATUS_REG, &status);
	if (rv)
		return rv;

	rv = read_reg(port, SYV682X_CONTROL_1_REG, &control_1);
	if (rv)
		return rv;

	if (!syv682x_is_sink(control_1)
		|| (status & SYV682X_STATUS_VSAFE_0V)) {
		/*
		 * Disable both power paths,
		 * set HV_ILIM to 3.3A,
		 * set 5V_ILIM to 3.3A,
		 * set HV direction to sink,
		 * select HV channel.
		 */
		regval = SYV682X_CONTROL_1_PWR_ENB |
			(SYV682X_HV_ILIM_3_30 << SYV682X_HV_ILIM_BIT_SHIFT) |
			/* !SYV682X_CONTROL_1_HV_DR */
			SYV682X_CONTROL_1_CH_SEL;
		rv = write_reg(port, SYV682X_CONTROL_1_REG, regval);
		if (rv)
			return rv;
	} else {
		/* Dead battery mode, or an existing PD contract is in place */
		rv = syv682x_vbus_sink_enable(port, 1);
		if (rv)
			return rv;
	}

#ifdef CONFIG_USB_PD_MAX_SINGLE_SOURCE_CURRENT
	initial_current_limit = CONFIG_USB_PD_MAX_SINGLE_SOURCE_CURRENT;
#else
	initial_current_limit = CONFIG_USB_PD_PULLUP;
#endif
	rv = syv682x_set_vbus_source_current_limit(port, initial_current_limit);
	if (rv)
		return rv;

	/*
	 * Set Control Reg 2 to defaults, plus enable smart discharge mode.
	 * The SYV682 automatically discharges under the following conditions:
	 * UVLO (under voltage lockout), channel shutdown, over current, over
	 * voltage, and thermal shutdown
	 */
	regval = (SYV682X_OC_DELAY_10MS << SYV682X_OC_DELAY_SHIFT)
		| (SYV682X_DSG_TIME_200MS << SYV682X_DSG_TIME_SHIFT)
		| (SYV682X_DSG_RON_200_OHM << SYV682X_DSG_RON_SHIFT)
		| SYV682X_CONTROL_2_SDSG;
	rv = write_reg(port, SYV682X_CONTROL_2_REG, regval);
	if (rv)
		return rv;

	/*
	 * Always set the over voltage setting to the maximum to support
	 * sinking from a 20V PD charger. The common PPC code doesn't provide
	 * any hooks for indicating what the currently negotiated voltage is.
	 */
	regval = (SYV682X_OVP_23_7 << SYV682X_OVP_BIT_SHIFT);
	rv = write_reg(port, SYV682X_CONTROL_3_REG, regval);
	if (rv)
		return rv;

	/*
	 * Remove Rd, connect CC1/CC2 lines to TCPC, and disable fast role
	 * swap.
	 */
	regval = SYV682X_CONTROL_4_CC1_BPS | SYV682X_CONTROL_4_CC2_BPS
		| SYV682X_CONTROL_4_CC_FRS;
	rv = write_reg(port, SYV682X_CONTROL_4_REG, regval);
	if (rv)
		return rv;

	return EC_SUCCESS;
}

const struct ppc_drv syv682x_drv = {
	.init = &syv682x_init,
	.is_sourcing_vbus = &syv682x_is_sourcing_vbus,
	.vbus_sink_enable = &syv682x_vbus_sink_enable,
	.vbus_source_enable = &syv682x_vbus_source_enable,
#ifdef CONFIG_CMD_PPC_DUMP
	.reg_dump = &syv682x_dump,
#endif /* defined(CONFIG_CMD_PPC_DUMP) */
#ifdef CONFIG_USB_PD_VBUS_DETECT_PPC
	.is_vbus_present = &syv682x_is_vbus_present,
#endif /* defined(CONFIG_USB_PD_VBUS_DETECT_PPC) */
	.set_vbus_source_current_limit = &syv682x_set_vbus_source_current_limit,
	.discharge_vbus = &syv682x_discharge_vbus,
#ifdef CONFIG_USBC_PPC_POLARITY
	.set_polarity = &syv682x_set_polarity,
#endif
#ifdef CONFIG_USBC_PPC_VCONN
	.set_vconn = &syv682x_set_vconn,
#endif
};
