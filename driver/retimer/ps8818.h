/* Copyright 2019 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * PS8818 retimer.
 */

#ifndef __CROS_EC_USB_RETIMER_PS8818_H
#define __CROS_EC_USB_RETIMER_PS8818_H

#define PS8818_I2C_ADDR_FLAGS	0x28

/*
 * PAGE 0 Register Definitions
 */
#define PS8818_REG_PAGE0	0x00

#define PS8818_REG0_FLIP		0x00
#define PS8818_FLIP_CONFIG			BIT(7)
#define PS8818_FLIP_NON_RESERVED_MASK		0xE0

#define PS8818_REG0_MODE		0x01
#define PS8818_MODE_DP_ENABLE			BIT(7)
#define PS8818_MODE_USB_ENABLE			BIT(6)
#define PS8818_MODE_NON_RESERVED_MASK		0xC0

#define PS8818_REG0_DPHPD_CONFIG	0x02
#define PS8818_DPHPD_CONFIG_INHPD_DISABLE	BIT(7)
#define PS8818_DPHPD_PLUGGED			BIT(6)
#define PS8818_DPHPD_NON_RESERVED_MASK		0xFC

/*
 * PAGE 1 Register Definitions
 */
#define PS8818_REG_PAGE1	0x01

#define PS8818_REG1_APTX1EQ_10G_LEVEL	0x00
#define PS8818_REG1_APTX2EQ_10G_LEVEL	0x02
#define PS8818_REG1_CRX1EQ_10G_LEVEL	0x08
#define PS8818_REG1_CRX2EQ_10G_LEVEL	0x0A
#define PS8818_REG1_APTX1EQ_5G_LEVEL	0x70
#define PS8818_REG1_APTX2EQ_5G_LEVEL	0x72
#define PS8818_REG1_CRX1EQ_5G_LEVEL	0x78
#define PS8818_REG1_CRX2EQ_5G_LEVEL	0x7A
#define PS8818_EQ_LEVEL_UP_9DB			(0)
#define PS8818_EQ_LEVEL_UP_10DB			(1)
#define PS8818_EQ_LEVEL_UP_12DB			(2)
#define PS8818_EQ_LEVEL_UP_13DB			(3)
#define PS8818_EQ_LEVEL_UP_16DB			(4)
#define PS8818_EQ_LEVEL_UP_17DB			(5)
#define PS8818_EQ_LEVEL_UP_18DB			(6)
#define PS8818_EQ_LEVEL_UP_19DB			(7)
#define PS8818_EQ_LEVEL_UP_20DB			(8)
#define PS8818_EQ_LEVEL_UP_21DB			(9)
#define PS8818_EQ_LEVEL_UP_MASK			(0x0F)

#define PS8818_REG1_DPEQ_LEVEL		0xB6
#define PS8818_DPEQ_LEVEL_UP_9DB		(0 << 3)
#define PS8818_DPEQ_LEVEL_UP_10DB		(1 << 3)
#define PS8818_DPEQ_LEVEL_UP_12DB		(2 << 3)
#define PS8818_DPEQ_LEVEL_UP_13DB		(3 << 3)
#define PS8818_DPEQ_LEVEL_UP_16DB		(4 << 3)
#define PS8818_DPEQ_LEVEL_UP_17DB		(5 << 3)
#define PS8818_DPEQ_LEVEL_UP_18DB		(6 << 3)
#define PS8818_DPEQ_LEVEL_UP_19DB		(7 << 3)
#define PS8818_DPEQ_LEVEL_UP_20DB		(8 << 3)
#define PS8818_DPEQ_LEVEL_UP_21DB		(9 << 3)
#define PS8818_DPEQ_LEVEL_UP_MASK		(0x0F << 3)

/*
 * PAGE 2 Register Definitions
 */
#define PS8818_REG_PAGE2	0x02

#define PS8818_REG2_TX_STATUS		0x42
#define PS8818_REG2_RX_STATUS		0x46
#define PS8818_STATUS_NORMAL_OPERATION		BIT(7)
#define PS8818_STATUS_10_GBPS			BIT(5)

#define PS8818_REG2_LINK_BW_SET		0x60
#define PS8818_LINK_BW_SET_1_62_GBPS		0x06
#define PS8818_LINK_BW_SET_2_7_GBPS		0x0A
#define PS8818_LINK_BW_SET_5_4_GBPS		0x14
#define PS8818_LINK_BW_SET_8_1_GBPS		0x1E
#define PS8818_LINK_BW_SET_MASK			0xFF

#define PS8818_REG2_LANE_COUNT_SET	0x61
#define PS8818_LANE_COUNT_SET_1_LANE		0x01
#define PS8818_LANE_COUNT_SET_2_LANE		0x02
#define PS8818_LANE_COUNT_SET_4_LANE		0x04
#define PS8818_LANE_COUNT_SET_MASK		0x1F

extern const struct usb_retimer_driver ps8818_usb_retimer;

int ps8818_detect(int port);

int ps8818_i2c_read(int port, int page, int offset, int *data);
int ps8818_i2c_write(int port, int page, int offset, int data);
int ps8818_i2c_field_update8(int port, int page, int offset,
			     uint8_t field_mask, uint8_t set_value);

#endif /* __CROS_EC_USB_RETIMER_PS8818_H */