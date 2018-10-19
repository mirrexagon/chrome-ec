/* Copyright 2016 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Reef board configuration */

#ifndef __CROS_EC_BOARD_H
#define __CROS_EC_BOARD_H

/*
 * By default, enable all console messages excepted HC, ACPI and event:
 * The sensor stack is generating a lot of activity.
 */
#define CC_DEFAULT     (CC_ALL & ~(CC_MASK(CC_EVENTS) | CC_MASK(CC_LPC)))
#undef CONFIG_HOSTCMD_DEBUG_MODE
#define CONFIG_HOSTCMD_DEBUG_MODE HCDEBUG_OFF


/* EC console commands  */
#define CONFIG_CMD_ACCELS
#define CONFIG_CMD_ACCEL_INFO
#define CONFIG_CMD_BATT_MFG_ACCESS
#define CONFIG_CMD_CHARGER_ADC_AMON_BMON
#define CONFIG_CHARGER_SENSE_RESISTOR		10
#define CONFIG_CHARGER_SENSE_RESISTOR_AC	10
#define BD9995X_IOUT_GAIN_SELECT \
		BD9995X_CMD_PMON_IOUT_CTRL_SET_IOUT_GAIN_SET_20V

#define CONFIG_CHARGER_PSYS_READ
#define BD9995X_PSYS_GAIN_SELECT \
		BD9995X_CMD_PMON_IOUT_CTRL_SET_PMON_GAIN_SET_02UAW

#define CONFIG_CMD_I2C_STRESS_TEST
#define CONFIG_CMD_I2C_STRESS_TEST_ACCEL
#define CONFIG_CMD_I2C_STRESS_TEST_ALS
#define CONFIG_CMD_I2C_STRESS_TEST_BATTERY
#define CONFIG_CMD_I2C_STRESS_TEST_CHARGER
#define CONFIG_CMD_I2C_STRESS_TEST_TCPC

/* Battery */
#define CONFIG_BATTERY_DEVICE_CHEMISTRY  "LION"
#define CONFIG_BATTERY_CUT_OFF
#define CONFIG_BATTERY_PRESENT_CUSTOM
#define CONFIG_BATTERY_SMART

/* Charger */
#define CONFIG_CHARGE_MANAGER
#define CONFIG_CHARGE_RAMP_SW
#define CONFIG_CHARGER
#define CONFIG_CHARGER_V2
#define CONFIG_CHARGER_BD9995X
#define CONFIG_CHARGER_BD9995X_CHGEN
#define CONFIG_CHARGER_DISCHARGE_ON_AC
#define CONFIG_CHARGER_INPUT_CURRENT 512
#define CONFIG_CHARGER_LIMIT_POWER_THRESH_BAT_PCT 1
#define CONFIG_CHARGER_LIMIT_POWER_THRESH_CHG_MW 18000
#define CONFIG_CHARGER_MAINTAIN_VBAT
#define CONFIG_CHARGER_MIN_BAT_PCT_FOR_POWER_ON 1
#define CONFIG_USB_CHARGER
#define CONFIG_CHARGER_PROFILE_OVERRIDE
#define CONFIG_CHARGER_PROFILE_OVERRIDE_COMMON
#undef  CONFIG_CHARGER_PROFILE_VOLTAGE_RANGES
#define CONFIG_CHARGER_PROFILE_VOLTAGE_RANGES 3
#define CONFIG_CHARGE_MANAGER_EXTERNAL_POWER_LIMIT

/* USB-A config */
#define CONFIG_USB_PORT_POWER_SMART
#define CONFIG_USB_PORT_POWER_SMART_DEFAULT_MODE USB_CHARGE_MODE_CDP
#define CONFIG_USB_PORT_POWER_SMART_SIMPLE
#undef CONFIG_USB_PORT_POWER_SMART_PORT_COUNT
#define CONFIG_USB_PORT_POWER_SMART_PORT_COUNT 1
#define GPIO_USB1_ILIM_SEL GPIO_USB_A_CHARGE_EN_L
#define GPIO_USB_CTL1 GPIO_EN_PP5000

#define CONFIG_TABLET_MODE

/* USB PD config */
#define CONFIG_CMD_PD_CONTROL
#define CONFIG_USB_PD_ALT_MODE
#define CONFIG_USB_PD_ALT_MODE_DFP
#define CONFIG_USB_PD_DUAL_ROLE
#define CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
#define CONFIG_USB_PD_DISCHARGE_TCPC
#define CONFIG_USB_PD_LOGGING
#define CONFIG_USB_PD_MAX_SINGLE_SOURCE_CURRENT TYPEC_RP_3A0
#define CONFIG_USB_PD_PORT_COUNT 2
#define CONFIG_USB_PD_VBUS_DETECT_CHARGER
#define CONFIG_USB_PD_TCPC_LOW_POWER
#define CONFIG_USB_PD_TCPM_MUX  /* for both PS8751 and ANX3429 */
#define CONFIG_USB_PD_TCPM_ANX3429 /* Silicon on Reef is ANX3429 */
#define CONFIG_USB_PD_TCPM_PS8751
#define CONFIG_USB_PD_TCPM_TCPCI
#define CONFIG_USB_PD_TRY_SRC
#define CONFIG_USB_POWER_DELIVERY
#define CONFIG_USB_PD_COMM_LOCKED

#define CONFIG_USBC_SS_MUX
#define CONFIG_USBC_SS_MUX_DFP_ONLY
#define CONFIG_USBC_VCONN
#define CONFIG_USBC_VCONN_SWAP

/* SoC / PCH */
#define CONFIG_HOSTCMD_LPC
#define CONFIG_CHIPSET_APOLLOLAKE
#define CONFIG_CHIPSET_RESET_HOOK
#define CONFIG_POWER_BUTTON
#define CONFIG_POWER_BUTTON_X86
#define CONFIG_POWER_COMMON
#define CONFIG_POWER_S0IX
#define CONFIG_POWER_TRACK_HOST_SLEEP_STATE

/* EC */
#define CONFIG_ADC
#define CONFIG_BOARD_VERSION_CUSTOM
#define CONFIG_EXTPOWER_GPIO
#undef   CONFIG_EXTPOWER_DEBOUNCE_MS
#define  CONFIG_EXTPOWER_DEBOUNCE_MS 1000
#define CONFIG_FPU
#define CONFIG_HOSTCMD_FLASH_SPI_INFO
#define CONFIG_I2C
#define CONFIG_I2C_MASTER
#define CONFIG_KEYBOARD_BOARD_CONFIG
#define CONFIG_KEYBOARD_PROTOCOL_8042
#define CONFIG_KEYBOARD_COL2_INVERTED
#define CONFIG_KEYBOARD_PWRBTN_ASSERTS_KSI2
#define CONFIG_LED_COMMON
#define CONFIG_LID_SWITCH
#define CONFIG_LOW_POWER_IDLE
#define CONFIG_LTO
#define CONFIG_POWER_SIGNAL_INTERRUPT_STORM_DETECT_THRESHOLD 30
#define CONFIG_PWM
#define CONFIG_TEMP_SENSOR
#define CONFIG_THERMISTOR_NCP15WB
#define CONFIG_DPTF
#define CONFIG_SCI_GPIO GPIO_PCH_SCI_L
#define CONFIG_VOLUME_BUTTONS
#define GPIO_VOLUME_DOWN_L GPIO_EC_VOLDN_BTN_ODL
#define GPIO_VOLUME_UP_L GPIO_EC_VOLUP_BTN_ODL
#define CONFIG_VBOOT_HASH
#define CONFIG_BACKLIGHT_LID
#define CONFIG_WIRELESS
#define CONFIG_WIRELESS_SUSPEND EC_WIRELESS_SWITCH_WLAN_POWER
#define CONFIG_WLAN_POWER_ACTIVE_LOW
#define  WIRELESS_GPIO_WLAN_POWER GPIO_WIRELESS_GPIO_WLAN_POWER
#define CONFIG_PWR_STATE_DISCHARGE_FULL

/*
 * During shutdown sequence TPS65094x PMIC turns off the sensor rails
 * asynchronously to the EC. If we access the sensors when the sensor power
 * rails are off we get I2C errors. To avoid this issue, defer switching
 * the sensors rate if in S3. By the time deferred function is serviced if
 * the chipset is in S5 we can back out from switching the sensor rate.
 *
 * Time taken by V1P8U rail to go down from S3 is 30ms to 60ms hence defer
 * the sensor switching after 60ms.
 */
#undef CONFIG_MOTION_SENSE_SUSPEND_DELAY_US
#define CONFIG_MOTION_SENSE_SUSPEND_DELAY_US (MSEC * 60)

/*
 * MEC1701H loads firmware using QMSPI controller
 * CONFIG_SPI_FLASH_PORT is the index into
 * spi_devices[] in board.c
 */
#define CONFIG_SPI_FLASH_PORT 0
#define CONFIG_SPI_FLASH

#define CONFIG_FLASH_SIZE 524288
#define CONFIG_SPI_FLASH_REGS
#define CONFIG_SPI_FLASH_W25Q40	/* FIXME: Should be GD25LQ40? */

/*
 * Enable 1 slot of secure temporary storage to support
 * suspend/resume with read/write memory training.
 */
#define CONFIG_VSTORE
#define CONFIG_VSTORE_SLOT_COUNT 1

/* Optional feature - used by MCHP */
#define CONFIG_CLOCK_CRYSTAL /* interposer board uses parallel crystal */
#define CONFIG_WATCHDOG_HELP /* required for MCHP MEC17xx */
#define CONFIG_BOARD_PRE_INIT

/* I2C ports */
#define I2C_CONTROLLER_COUNT	4
#define I2C_PORT_COUNT		5

#define I2C_PORT_GYRO			MCHP_I2C_PORT6
#define I2C_PORT_LID_ACCEL		MCHP_I2C_PORT7
#define I2C_PORT_ALS			MCHP_I2C_PORT7
#define I2C_PORT_BARO			MCHP_I2C_PORT7
#define I2C_PORT_BATTERY		MCHP_I2C_PORT3
#define I2C_PORT_CHARGER		MCHP_I2C_PORT3
/* Accelerometer and Gyroscope are the same device. */
#define I2C_PORT_ACCEL			I2C_PORT_GYRO

/* Sensors */
#define CONFIG_MKBP_EVENT
#define CONFIG_MKBP_USE_HOST_EVENT
#define CONFIG_ACCELGYRO_BMI160
#define CONFIG_ACCEL_INTERRUPTS
#define CONFIG_ACCELGYRO_BMI160_INT_EVENT TASK_EVENT_CUSTOM(4)
#define CONFIG_MAG_BMI160_BMM150
#define BMI160_SEC_ADDR BMM150_ADDR0	/* 8-bit address */
#define CONFIG_MAG_CALIBRATE
#define CONFIG_ACCEL_KX022
#define CONFIG_ALS_OPT3001
#define CONFIG_BARO_BMP280
#define CONFIG_LID_ANGLE
#define CONFIG_LID_ANGLE_UPDATE
#define CONFIG_LID_ANGLE_SENSOR_BASE BASE_ACCEL
#define CONFIG_LID_ANGLE_SENSOR_LID LID_ACCEL

/* FIFO size is in power of 2. */
#define CONFIG_ACCEL_FIFO 1024

/* Depends on how fast the AP boots and typical ODRs */
#define CONFIG_ACCEL_FIFO_THRES (CONFIG_ACCEL_FIFO / 3)


#ifndef __ASSEMBLER__

#include "gpio_signal.h"
#include "registers.h"

/* ADC signal */
enum adc_channel {
	ADC_TEMP_SENSOR_CHARGER,	/* ADC0 */
	ADC_TEMP_SENSOR_AMB,		/* ADC1 */
	ADC_BOARD_ID,			/* ADC2 */
	ADC_CH_COUNT
};

enum pwm_channel {
	PWM_CH_LED_GREEN = 0,
	PWM_CH_LED_RED,
	/* Number of PWM channels */
	PWM_CH_COUNT
};

enum power_signal {
#ifdef CONFIG_POWER_S0IX
	X86_SLP_S0_N,
#endif
	X86_RSMRST_N,
	X86_SLP_S3_N,
	X86_SLP_S4_N,
	X86_SUSPWRDNACK,

	X86_ALL_SYS_PG,		/* PMIC_EC_PWROK_OD */
	X86_PGOOD_PP3300,	/* GPIO_PP3300_PG */
	X86_PGOOD_PP5000,	/* GPIO_PP5000_PG */

	/* Number of X86 signals */
	POWER_SIGNAL_COUNT
};

enum temp_sensor_id {
	TEMP_SENSOR_BATTERY = 0,
	TEMP_SENSOR_AMBIENT,
	TEMP_SENSOR_CHARGER,
	TEMP_SENSOR_COUNT
};

/* MCHP - new Reef code not using TASK_ALS */
/*
 * For backward compatibility, to report ALS via ACPI,
 * Define the number of ALS sensors: motion_sensor copy the data to the ALS
 * memmap region.
 */
#define CONFIG_ALS
#define ALS_COUNT 1

/*
 * Motion sensors:
 * When reading through IO memory is set up for sensors (LPC is used),
 * the first 2 entries must be accelerometers, then gyroscope.
 * For BMI160, accel, gyro and compass sensors must be next to each other.
 */
enum sensor_id {
	LID_ACCEL = 0,
	BASE_ACCEL,
	BASE_GYRO,
	BASE_MAG,
	BASE_BARO,
	LID_ALS,	/* firmware-reef-9042.B doesn't have this */
};

enum reef_board_version {
	BOARD_VERSION_UNKNOWN = -1,
	BOARD_VERSION_1,
	BOARD_VERSION_2,
	BOARD_VERSION_3,
	BOARD_VERSION_4,
	BOARD_VERSION_5,
	BOARD_VERSION_6,
	BOARD_VERSION_7,
	BOARD_VERSION_8,
	BOARD_VERSION_COUNT,
};

/* TODO: determine the following board specific type-C power constants */
/* FIXME(dhendrix): verify all of the below PD_* numbers */
/*
 * delay to turn on the power supply max is ~16ms.
 * delay to turn off the power supply max is about ~180ms.
 */
#define PD_POWER_SUPPLY_TURN_ON_DELAY  30000  /* us */
#define PD_POWER_SUPPLY_TURN_OFF_DELAY 250000 /* us */

/* delay to turn on/off vconn */
#define PD_VCONN_SWAP_DELAY 5000 /* us */

/* Define typical operating power and max power */
#define PD_OPERATING_POWER_MW 15000
#define PD_MAX_POWER_MW       45000
#define PD_MAX_CURRENT_MA     3000
#define PD_MAX_VOLTAGE_MV     20000

/* Reset PD MCU */
void board_reset_pd_mcu(void);

int board_get_version(void);

void board_set_tcpc_power_mode(int port, int mode);
void board_print_tcpc_fw_version(int port);

/* Map I2C port to controller */
int board_i2c_p2c(int port);

/* Return the two slave addresses the specified
 * controller will respond to when controller
 * is acting as a slave.
 * b[6:0]  = b[7:1] of I2C address 1
 * b[14:8] = b[7:1] of I2C address 2
 */
uint16_t board_i2c_slave_addrs(int controller);

/* MCHP - firwmare-reef-9042.B does have LID_ALS bit
 * because its using TASK_ALS ?
 */
/* Sensors without hardware FIFO are in forced mode */
#define CONFIG_ACCEL_FORCE_MODE_MASK \
	((1 << LID_ACCEL) | (1 << BASE_BARO) | (1 << LID_ALS))

#endif /* !__ASSEMBLER__ */

#endif /* __CROS_EC_BOARD_H */
