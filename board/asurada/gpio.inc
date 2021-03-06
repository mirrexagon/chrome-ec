/* -*- mode:c -*-
 *
 * Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Declare symbolic names for all the GPIOs that we care about.
 * Note: Those with interrupt handlers must be declared first. */

/* Wake Source interrupts */
GPIO_INT(POWER_BUTTON_L,     PIN(E, 4), GPIO_INT_BOTH | GPIO_PULL_UP |
	 GPIO_HIB_WAKE_HIGH, power_button_interrupt) /* H1_EC_PWR_BTN_ODL */
GPIO_INT(LID_OPEN,           PIN(E, 2), GPIO_INT_BOTH | GPIO_PULL_DOWN |
         GPIO_HIB_WAKE_HIGH, lid_interrupt)
GPIO_INT(TABLET_MODE_L,      PIN(J, 7), GPIO_INT_BOTH | GPIO_PULL_UP,
         gmr_tablet_switch_isr)

/* Chipset interrupts */
GPIO_INT(AP_EC_WARM_RST_REQ, PIN(D, 3), GPIO_INT_RISING | GPIO_SEL_1P8V,
         chipset_reset_request_interrupt)

/* Power sequencing interrupts */
GPIO_INT(AP_EC_WATCHDOG_L, PIN(C, 7), GPIO_INT_BOTH | GPIO_SEL_1P8V,
         chipset_watchdog_interrupt)
GPIO_INT(AP_IN_SLEEP_L, PIN(F, 2),
         GPIO_INT_BOTH | GPIO_PULL_DOWN | GPIO_SEL_1P8V, power_signal_interrupt)
GPIO_INT(PMIC_EC_PWRGD, PIN(F, 3),
         GPIO_INT_BOTH | GPIO_PULL_DOWN | GPIO_SEL_1P8V, power_signal_interrupt)

/* Sensor Interrupts */
GPIO_INT(BASE_IMU_INT_L,     PIN(J, 2), GPIO_INT_FALLING | GPIO_SEL_1P8V,
         bmi160_interrupt)
GPIO_INT(LID_ACCEL_INT_L,    PIN(J, 3), GPIO_INT_FALLING | GPIO_SEL_1P8V,
         lis2dw12_interrupt)

/* USB-C interrupts */
GPIO_INT(USB_C0_PPC_INT_ODL, PIN(D, 1), GPIO_INT_BOTH, ppc_interrupt)

/* Volume button interrupts */

/* Other interrupts */
GPIO_INT(AC_PRESENT,         PIN(M, 2),  GPIO_INT_BOTH | GPIO_HIB_WAKE_HIGH,
         extpower_interrupt) /* ACOK_OD */
GPIO_INT(UART1_RX,           PIN(B, 0), GPIO_INT_FALLING,
         uart_deepsleep_interrupt) /* UART_DEBUG_TX_EC_RX */
GPIO_INT(WP,                 PIN(I, 4), GPIO_INT_BOTH,
         switch_interrupt) /* EC_FLASH_WP_OD */
GPIO_INT(VOLUME_DOWN_L,      PIN(D, 6), GPIO_INT_BOTH | GPIO_PULL_UP,
         button_interrupt) /* EC_VOLDN_BTN_ODL */
GPIO_INT(VOLUME_UP_L,        PIN(D, 5), GPIO_INT_BOTH | GPIO_PULL_UP,
         button_interrupt) /* EC_VOLUP_BTN_ODL */
GPIO_INT(SPI0_CS,            PIN(M, 5), GPIO_INT_FALLING | GPIO_PULL_UP,
         spi_event)        /* SPI slave Chip Select -- AP_SPI_EC_CS_L */
GPIO_INT(X_EC_GPIO2,         PIN(B, 2), GPIO_ODR_HIGH, x_ec_interrupt)

/* Power Sequencing Signals */
GPIO(EC_PMIC_EN_ODL,         PIN(D, 0), GPIO_ODR_HIGH | GPIO_SEL_1P8V)
GPIO(EC_PMIC_WATCHDOG_L,     PIN(H, 0), GPIO_ODR_LOW | GPIO_SEL_1P8V)
/* b/154775121: default on, because PPCs need the PP5000 to init in G3. */
GPIO(EN_PP5000_A,            PIN(C, 6), GPIO_OUT_HIGH)
GPIO(PG_MT6315_PROC_ODL,     PIN(E, 1), GPIO_INPUT)
GPIO(PG_MT6360_ODL,          PIN(F, 1), GPIO_INPUT)
GPIO(PG_PP5000_A_ODL,        PIN(A, 6), GPIO_INPUT)
GPIO(PMIC_FORCE_RESET,       PIN(E, 3), GPIO_OUT_LOW | GPIO_SEL_1P8V)
GPIO(SYS_RST_ODL,            PIN(B, 6), GPIO_ODR_LOW)
GPIO(EC_BL_EN_OD,            PIN(B, 5), GPIO_ODR_LOW)

/* MKBP event synchronization */
GPIO(EC_INT_L,               PIN(E, 6), GPIO_ODR_HIGH) /* EC_AP_INT_ODL */

/* USB and USBC Signals */

/* Misc Signals */
GPIO(EC_BATT_PRES_ODL,       PIN(C, 0), GPIO_INPUT)

/* I2C pins - Alternate function below configures I2C module on these pins */
GPIO(I2C_A_SCL,              PIN(B, 3), GPIO_INPUT) /* I2C_CHG_BATT_SCL */
GPIO(I2C_A_SDA,              PIN(B, 4), GPIO_INPUT) /* I2C_CHG_BATT_SDA */
GPIO(I2C_B_SCL,              PIN(C, 1), GPIO_INPUT) /* I2C_SENSOR_SCL */
GPIO(I2C_B_SDA,              PIN(C, 2), GPIO_INPUT) /* I2C_SENSOR_SDA */
GPIO(I2C_C_SCL,              PIN(F, 6), GPIO_INPUT) /* I2C_USB_C0_SCL */
GPIO(I2C_C_SDA,              PIN(F, 7), GPIO_INPUT) /* I2C_USB_C0_SCL */
GPIO(I2C_E_SCL,              PIN(E, 0), GPIO_INPUT) /* I2C_USB_C1_SCL */
GPIO(I2C_E_SDA,              PIN(E, 7), GPIO_INPUT) /* I2C_USB_C1_SDA */

/* SPI pins - Alternate function below configures SPI module on these pins */

/* NC / TP */

/* Keyboard pins */

/* Subboards HDMI/TYPEC */
GPIO(EC_X_GPIO1,             PIN(H, 4), GPIO_OUT_HIGH)
GPIO(EC_X_GPIO3,             PIN(J, 1), GPIO_INPUT)

/* Alternate functions GPIO definitions */
ALTERNATE(PIN_MASK(B, 0x18), 1, MODULE_I2C, 0) /* I2C A */
ALTERNATE(PIN_MASK(C, 0x06), 1, MODULE_I2C, 0) /* I2C B */
ALTERNATE(PIN_MASK(F, 0xC0), 1, MODULE_I2C, 0) /* I2C C */
ALTERNATE(PIN_MASK(E, 0x81), 1, MODULE_I2C, 0) /* I2C E */

/* UART */
ALTERNATE(PIN_MASK(B, 0x03), 1, MODULE_UART, 0) /* EC to Servo */

/* PWM */
ALTERNATE(PIN_MASK(A, 0x0F), 1, MODULE_PWM, 0) /* PWM 0~3 */

/* ADC */
ALTERNATE(PIN_MASK(I, 0xEF), 0, MODULE_ADC, 0) /* ADC 0,1,2,3,5,6,7 */

/* SPI */
ALTERNATE(PIN_MASK(M, 0x33), 0, MODULE_SPI, 0) /* SPI */

/* Power Switch Logic (PSL) inputs */
GPIO(EC_RST_ODL,             PIN(L, 1), GPIO_INPUT)

/* Unimplemented Pins  */
UNIMPLEMENTED(PCH_SMI_L)
UNIMPLEMENTED(PCH_SCI_L)
UNIMPLEMENTED(PCH_WAKE_L)
UNIMPLEMENTED(PCH_PLTRST_L)

#define UNIMPLEMENTED_GPIO(name, pin) UNIMPLEMENTED(name)

UNIMPLEMENTED_GPIO(PACKET_MODE_EN,         PIN(M, 6))
UNIMPLEMENTED_GPIO(EC_GPM3,                PIN(M, 3))
UNIMPLEMENTED_GPIO(AP_SPI_EC_MISO_R,       PIN(M, 1))
UNIMPLEMENTED_GPIO(AP_SPI_EC_MOSI,         PIN(M, 0))
UNIMPLEMENTED_GPIO(AP_SPI_EC_CLK,          PIN(M, 4))
UNIMPLEMENTED_GPIO(SET_VMC_VOLT_AT_1V8,    PIN(D, 4))
UNIMPLEMENTED_GPIO(EN_PP3000_VMC_PMU,      PIN(D, 2))
UNIMPLEMENTED_GPIO(PWR_LED_ODL,            PIN(A, 0))
UNIMPLEMENTED_GPIO(EC_LED_PWM1,            PIN(A, 1))
UNIMPLEMENTED_GPIO(EC_LED_PWM2,            PIN(A, 2))
UNIMPLEMENTED_GPIO(EC_LED_PWM3,            PIN(A, 3))
UNIMPLEMENTED_GPIO(USB_A0_FAULT_ODL,       PIN(A, 7))
UNIMPLEMENTED_GPIO(USB_C1_MUX_INT_ODL,     PIN(E, 5))
UNIMPLEMENTED_GPIO(CHARGER_PROCHOT_ODL,    PIN(C, 3))
UNIMPLEMENTED_GPIO(ENTERING_RW,            PIN(C, 5)) /* EC_ENTERING_RW */
UNIMPLEMENTED_GPIO(EC_TEMP_SENSOR_SUBPMIC, PIN(I, 0))
UNIMPLEMENTED_GPIO(EC_BOARD_ID0,           PIN(I, 1))
UNIMPLEMENTED_GPIO(EC_BOARD_ID1,           PIN(I, 2))
UNIMPLEMENTED_GPIO(TEMP_SENSOR_AMB,        PIN(I, 3))
UNIMPLEMENTED_GPIO(TEMP_SENSOR_CHARGER,    PIN(I, 5))
UNIMPLEMENTED_GPIO(CHARGER_PMON,           PIN(I, 6))
UNIMPLEMENTED_GPIO(TEMP_SENSOR_AP,         PIN(I, 7))
UNIMPLEMENTED_GPIO(EC_DPBRDG_HPD_ODL,      PIN(J, 0))
UNIMPLEMENTED_GPIO(USB_C1_BC12_INT_L,      PIN(J, 4))
UNIMPLEMENTED_GPIO(EN_USB_C0_BC12_DET,     PIN(J, 5))
UNIMPLEMENTED_GPIO(ALS_RGB_INT_ODL,        PIN(F, 0))
UNIMPLEMENTED_GPIO(EC_USB_C0_CC1,          PIN(F, 4))
UNIMPLEMENTED_GPIO(EC_USB_C0_CC2,          PIN(F, 5))
UNIMPLEMENTED_GPIO(EC_USB_C1_CC1,          PIN(H, 1))
UNIMPLEMENTED_GPIO(EC_USB_C1_CC2,          PIN(H, 2))
UNIMPLEMENTED_GPIO(USB_C0_FRS_EN,          PIN(H, 3))
UNIMPLEMENTED_GPIO(EN_EC_ID_DOL,           PIN(H, 5))
UNIMPLEMENTED_GPIO(PG_MT6315_GPU_ODL,      PIN(H, 6))
UNIMPLEMENTED_GPIO(EC_GPG2,                PIN(G, 2))
UNIMPLEMENTED_GPIO(EC_SPI_FLASH_MOSI,      PIN(G, 4))
UNIMPLEMENTED_GPIO(EC_SPI_FLASH_MISO,      PIN(G, 5))
UNIMPLEMENTED_GPIO(EC_SPI_FLASH_CLK,       PIN(G, 6))
UNIMPLEMENTED_GPIO(EC_SPI_FLASH_CS_L,      PIN(G, 7))
UNIMPLEMENTED_GPIO(DP_AUX_PATH_SEL,        PIN(G, 0))
UNIMPLEMENTED_GPIO(EN_PP3000_SD_U,         PIN(G, 1))
UNIMPLEMENTED_GPIO(UART1_TX,               PIN(B, 1)) /* UART_EC_TX_DEBUG_RX */
UNIMPLEMENTED_GPIO(EN_PP5000_USB_A0_VBUS,  PIN(B, 7))
UNIMPLEMENTED_GPIO(CCD_MODE_ODL,           PIN(C, 4))
UNIMPLEMENTED_GPIO(CLK_EC_RTC_32K,         PIN(J, 6))

#undef UNIMPLEMENTED_GPIO
