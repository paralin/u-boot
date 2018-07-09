/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2015 Vishnu Patekar <vishnupatekar0510@gmail.com>
 *
 * X-Powers AXP818 Power Management IC driver
 */

#define AXP818_CHIP_ID		0x03

#define AXP818_OUTPUT_CTRL1	0x10
#define AXP818_OUTPUT_CTRL1_DCDC1_EN	(1 << 0)
#define AXP818_OUTPUT_CTRL1_DCDC2_EN	(1 << 1)
#define AXP818_OUTPUT_CTRL1_DCDC3_EN	(1 << 2)
#define AXP818_OUTPUT_CTRL1_DCDC4_EN	(1 << 3)
#define AXP818_OUTPUT_CTRL1_DCDC5_EN	(1 << 4)
#define AXP818_OUTPUT_CTRL1_DCDC6_EN	(1 << 5)
#define AXP818_OUTPUT_CTRL1_DCDC7_EN	(1 << 6)
#define AXP818_OUTPUT_CTRL2	0x12
#define AXP818_OUTPUT_CTRL2_ELDO1_EN	(1 << 0)
#define AXP818_OUTPUT_CTRL2_ELDO2_EN	(1 << 1)
#define AXP818_OUTPUT_CTRL2_ELDO3_EN	(1 << 2)
#define AXP818_OUTPUT_CTRL2_DLDO1_EN	(1 << 3)
#define AXP818_OUTPUT_CTRL2_DLDO2_EN	(1 << 4)
#define AXP818_OUTPUT_CTRL2_DLDO3_EN	(1 << 5)
#define AXP818_OUTPUT_CTRL2_DLDO4_EN	(1 << 6)
#define AXP818_OUTPUT_CTRL2_SW_EN	(1 << 7)
#define AXP818_OUTPUT_CTRL3	0x13
#define AXP818_OUTPUT_CTRL3_FLDO1_EN	(1 << 2)
#define AXP818_OUTPUT_CTRL3_FLDO2_EN	(1 << 3)
#define AXP818_OUTPUT_CTRL3_FLDO3_EN	(1 << 4)
#define AXP818_OUTPUT_CTRL3_ALDO1_EN	(1 << 5)
#define AXP818_OUTPUT_CTRL3_ALDO2_EN	(1 << 6)
#define AXP818_OUTPUT_CTRL3_ALDO3_EN	(1 << 7)

#define AXP818_DLDO1_CTRL	0x15
#define AXP818_DLDO2_CTRL	0x16
#define AXP818_DLDO3_CTRL	0x17
#define AXP818_DLDO4_CTRL	0x18
#define AXP818_ELDO1_CTRL	0x19
#define AXP818_ELDO2_CTRL	0x1a
#define AXP818_ELDO3_CTRL	0x1b
#define AXP818_FLDO1_CTRL	0x1c
#define AXP818_FLDO2_3_CTRL	0x1d
#define AXP818_FLDO2_3_CTRL_FLDO3_VOL	(1 << 4)
#define AXP818_DCDC1_CTRL	0x20
#define AXP818_DCDC2_CTRL	0x21
#define AXP818_DCDC3_CTRL	0x22
#define AXP818_DCDC4_CTRL	0x23
#define AXP818_DCDC5_CTRL	0x24
#define AXP818_DCDC6_CTRL	0x25
#define AXP818_DCDC7_CTRL	0x26

#define AXP818_ALDO1_CTRL	0x28
#define AXP818_ALDO2_CTRL	0x29
#define AXP818_ALDO3_CTRL	0x2a

#define AXP818_SHUTDOWN		0x32
#define AXP818_SHUTDOWN_POWEROFF	(1 << 7)

/* For axp_gpio.c */
#define AXP_POWER_STATUS		0x00
#define AXP_POWER_STATUS_VBUS_PRESENT		(1 << 5)
#define AXP_POWER_STATUS_VBUS_VALID		(1 << 4)
#define AXP_POWER_STATUS_VBAT_GT_3_5V		(1 << 3)
#define AXP_POWER_STATUS_VBUS_STARTUP		(1 << 0)

#define AXP_VBUS_IPSOUT			0x30
#define AXP_VBUS_IPSOUT_DRIVEBUS		(1 << 2)
#define AXP_MISC_CTRL			0x8f
#define AXP_MISC_CTRL_N_VBUSEN_FUNC		(1 << 4)
#define AXP_GPIO0_CTRL			0x90
#define AXP_GPIO0_LDO_CTRL		0x91
#define AXP_GPIO1_CTRL			0x92
#define AXP_GPIO_CTRL_OUTPUT_LOW	0x00 /* Drive pin low */
#define AXP_GPIO_CTRL_OUTPUT_HIGH	0x01 /* Drive pin high */
#define AXP_GPIO_CTRL_INPUT		0x02 /* Input */
#define AXP_GPIO_CTRL_LDO_ON		0x03 /* LDO on */
#define AXP_GPIO_STATE			0x94
#define AXP_GPIO_STATE_OFFSET		0

#define AXP_CHARGER_STATUS		0x01
#define AXP_CHARGER_STATUS_BAT_PRESENT	(1 << 5)
#define AXP_CHARGER_STATUS_BAT_PRESENT_VALID	(1 << 4)
#define AXP_CHARGER_STATUS_CHARGING	(1 << 6)
#define AXP_CHARGER_STATUS_OVER_TEMP	(1 << 7)

#define AXP_POWER_UP_DOWN_REASON	0x02
#define AXP_POWER_UP_REASON_POK		(1 << 0)
#define AXP_POWER_UP_REASON_CHARGER	(1 << 1)
#define AXP_POWER_UP_REASON_GLOBAL_RESET (1 << 3)
#define AXP_POWER_UP_REASON_COLD_RESET	(1 << 4)
#define AXP_POWER_DOWN_REASON_UVLO	(1 << 5)
#define AXP_POWER_DOWN_REASON_COLD_OFF 	(1 << 6)
#define AXP_POWER_DOWN_REASON_POK_OR	(1 << 7)

#define AXP_BC_GLOBAL			0x2c
#define AXP_BC_GLOBAL_EN		(1 << 0)

#define AXP_CHGLED			0x32
#define AXP_CHGLED_CTRL_CHARGER		(1 << 3)
#define AXP_CHGLED_SETUP_MASK		(0x3 << 4)
#define AXP_CHGLED_SETUP_OFF		(0 << 4)
#define AXP_CHGLED_SETUP_BLINK_FAST	(1 << 4)
#define AXP_CHGLED_SETUP_BLINK_SLOW	(2 << 4)
#define AXP_CHGLED_SETUP_ON		(3 << 4)

#define AXP_POK				0x36
#define AXP_POK_ON_MASK			(3 << 6)
#define AXP_POK_ON_128MS		(0 << 6)
#define AXP_POK_ON_1S			(1 << 6)
#define AXP_POK_ON_2S			(2 << 6)
#define AXP_POK_ON_3S			(3 << 6)
#define AXP_POK_OFF_MASK		(3 << 0)
#define AXP_POK_OFF_4S			(0 << 6)
#define AXP_POK_OFF_6S			(1 << 6)
#define AXP_POK_OFF_8S			(2 << 6)
#define AXP_POK_OFF_10S			(3 << 6)

#define AXP_CHARGER_CTRL3		0x35
#define AXP_CHARGER_CTRL3_VBUS_I_MASK	0xf0
#define AXP_CHARGER_CTRL3_VBUS_I_100	0x00
#define AXP_CHARGER_CTRL3_VBUS_I_500	0x10
#define AXP_CHARGER_CTRL3_VBUS_I_900	0x20
#define AXP_CHARGER_CTRL3_VBUS_I_1500	0x30
#define AXP_CHARGER_CTRL3_VBUS_I_2000	0x40
#define AXP_CHARGER_CTRL3_VBUS_I_2500	0x50
#define AXP_CHARGER_CTRL3_VBUS_I_3000	0x60
#define AXP_CHARGER_CTRL3_VBUS_I_3500	0x70
#define AXP_CHARGER_CTRL3_VBUS_I_4000	0x80

#define AXP_AD_BAT_VOLTAGE_MSB8		0x78
#define AXP_AD_BAT_VOLTAGE_LSB4		0x79
#define AXP_AD_BAT_CHG_CURRENT_MSB8	0x7a
#define AXP_AD_BAT_CHG_CURRENT_LSB4	0x7b
#define AXP_AD_BAT_DIS_CURRENT_MSB8	0x7c
#define AXP_AD_BAT_DIS_CURRENT_LSB4	0x7d

#define AXP_BATTERY_CAPACITY 		0xb9

int axp_gpio0_enable_ldo_set_voltage(u32 mV);

int axp_is_charging(bool* charging);
int axp_is_vbus_present(bool* vbus);
int axp_is_battery_present(bool* vbus);
int axp_clear_startup_reason(void);
int axp_get_battery_voltage(int* uV);
int axp_get_battery_current(int* uA);
int axp_battery_get_capacity(int* capacity);
int axp_usb_get_max_current(int* mA);
int axp_usb_set_max_current(int mA);
int axp_led_set_charger_controlled(void);
int axp_led_set(int mode);
int axp_pok_set_quick(void);
int axp_set_bc_en(void);
