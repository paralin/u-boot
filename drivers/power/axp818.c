// SPDX-License-Identifier: GPL-2.0+
/*
 * AXP818 driver based on AXP221 driver
 *
 *
 * (C) Copyright 2015 Vishnu Patekar <vishnuptekar0510@gmail.com>
 *
 * Based on axp221.c
 * (C) Copyright 2014 Hans de Goede <hdegoede@redhat.com>
 * (C) Copyright 2013 Oliver Schinagl <oliver@schinagl.nl>
 */

#include <common.h>
#include <command.h>
#include <errno.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pmic_bus.h>
#include <axp_pmic.h>

static u8 axp818_mvolt_to_cfg(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return  (mvolt - min) / div;
}

int axp_set_dcdc1(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp818_mvolt_to_cfg(mvolt, 1600, 3400, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP818_OUTPUT_CTRL1,
					AXP818_OUTPUT_CTRL1_DCDC1_EN);

	ret = pmic_bus_write(AXP818_DCDC1_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP818_OUTPUT_CTRL1,
				AXP818_OUTPUT_CTRL1_DCDC1_EN);
}

int axp_set_dcdc2(unsigned int mvolt)
{
	int ret;
	u8 cfg;

	if (mvolt >= 1220)
		cfg = 70 + axp818_mvolt_to_cfg(mvolt, 1220, 1300, 20);
	else
		cfg = axp818_mvolt_to_cfg(mvolt, 500, 1200, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP818_OUTPUT_CTRL1,
					AXP818_OUTPUT_CTRL1_DCDC2_EN);

	ret = pmic_bus_write(AXP818_DCDC2_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP818_OUTPUT_CTRL1,
				AXP818_OUTPUT_CTRL1_DCDC2_EN);
}

int axp_set_dcdc3(unsigned int mvolt)
{
	int ret;
	u8 cfg;

	if (mvolt >= 1220)
		cfg = 70 + axp818_mvolt_to_cfg(mvolt, 1220, 1300, 20);
	else
		cfg = axp818_mvolt_to_cfg(mvolt, 500, 1200, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP818_OUTPUT_CTRL1,
					AXP818_OUTPUT_CTRL1_DCDC3_EN);

	ret = pmic_bus_write(AXP818_DCDC3_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP818_OUTPUT_CTRL1,
				AXP818_OUTPUT_CTRL1_DCDC3_EN);
}

int axp_set_dcdc5(unsigned int mvolt)
{
	int ret;
	u8 cfg;

	if (mvolt >= 1140)
		cfg = 32 + axp818_mvolt_to_cfg(mvolt, 1140, 1840, 20);
	else
		cfg = axp818_mvolt_to_cfg(mvolt, 800, 1120, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP818_OUTPUT_CTRL1,
					AXP818_OUTPUT_CTRL1_DCDC5_EN);

	ret = pmic_bus_write(AXP818_DCDC5_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP818_OUTPUT_CTRL1,
				AXP818_OUTPUT_CTRL1_DCDC5_EN);
}

int axp_set_dcdc6(unsigned int mvolt)
{
	int ret;
	u8 cfg;

	if (mvolt >= 1120)
		cfg = 50 + axp818_mvolt_to_cfg(mvolt, 1120, 1520, 20);
	else
		cfg = axp818_mvolt_to_cfg(mvolt, 600, 1100, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP818_OUTPUT_CTRL1,
					AXP818_OUTPUT_CTRL1_DCDC6_EN);

	ret = pmic_bus_write(AXP818_DCDC6_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP818_OUTPUT_CTRL1,
				AXP818_OUTPUT_CTRL1_DCDC6_EN);
}

int axp_set_aldo(int aldo_num, unsigned int mvolt)
{
	int ret;
	u8 cfg;

	if (aldo_num < 1 || aldo_num > 3)
		return -EINVAL;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP818_OUTPUT_CTRL3,
				AXP818_OUTPUT_CTRL3_ALDO1_EN << (aldo_num - 1));

	cfg = axp818_mvolt_to_cfg(mvolt, 700, 3300, 100);
	ret = pmic_bus_write(AXP818_ALDO1_CTRL + (aldo_num - 1), cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP818_OUTPUT_CTRL3,
				AXP818_OUTPUT_CTRL3_ALDO1_EN << (aldo_num - 1));
}

/* TODO: re-work other AXP drivers to consolidate ALDO functions. */
int axp_set_aldo1(unsigned int mvolt)
{
	return axp_set_aldo(1, mvolt);
}

int axp_set_aldo2(unsigned int mvolt)
{
	return axp_set_aldo(2, mvolt);
}

int axp_set_aldo3(unsigned int mvolt)
{
	return axp_set_aldo(3, mvolt);
}

int axp_set_dldo(int dldo_num, unsigned int mvolt)
{
	int ret;
	u8 cfg;

	if (dldo_num < 1 || dldo_num > 4)
		return -EINVAL;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP818_OUTPUT_CTRL2,
				AXP818_OUTPUT_CTRL2_DLDO1_EN << (dldo_num - 1));

	cfg = axp818_mvolt_to_cfg(mvolt, 700, 3300, 100);
	if (dldo_num == 2 && mvolt > 3300)
		cfg += 1 + axp818_mvolt_to_cfg(mvolt, 3400, 4200, 200);
	ret = pmic_bus_write(AXP818_DLDO1_CTRL + (dldo_num - 1), cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP818_OUTPUT_CTRL2,
				AXP818_OUTPUT_CTRL2_DLDO1_EN << (dldo_num - 1));
}

int axp_set_eldo(int eldo_num, unsigned int mvolt)
{
	int ret;
	u8 cfg;

	if (eldo_num < 1 || eldo_num > 3)
		return -EINVAL;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP818_OUTPUT_CTRL2,
				AXP818_OUTPUT_CTRL2_ELDO1_EN << (eldo_num - 1));

	cfg = axp818_mvolt_to_cfg(mvolt, 700, 1900, 50);
	ret = pmic_bus_write(AXP818_ELDO1_CTRL + (eldo_num - 1), cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP818_OUTPUT_CTRL2,
				AXP818_OUTPUT_CTRL2_ELDO1_EN << (eldo_num - 1));
}

int axp_set_fldo(int fldo_num, unsigned int mvolt)
{
	int ret;
	u8 cfg;

	if (fldo_num < 1 || fldo_num > 3)
		return -EINVAL;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP818_OUTPUT_CTRL3,
				AXP818_OUTPUT_CTRL3_FLDO1_EN << (fldo_num - 1));

	if (fldo_num < 3) {
		cfg = axp818_mvolt_to_cfg(mvolt, 700, 1450, 50);
		ret = pmic_bus_write(AXP818_FLDO1_CTRL + (fldo_num - 1), cfg);
	} else {
		/*
		 * Special case for FLDO3, which is DCDC5 / 2 or FLDOIN / 2
		 * Since FLDOIN is unknown, test against DCDC5.
		 */
		if (mvolt * 2 == CONFIG_AXP_DCDC5_VOLT)
			ret = pmic_bus_clrbits(AXP818_FLDO2_3_CTRL,
					       AXP818_FLDO2_3_CTRL_FLDO3_VOL);
		else
			ret = pmic_bus_setbits(AXP818_FLDO2_3_CTRL,
					       AXP818_FLDO2_3_CTRL_FLDO3_VOL);
	}
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP818_OUTPUT_CTRL3,
				AXP818_OUTPUT_CTRL3_FLDO1_EN << (fldo_num - 1));
}

int axp_set_sw(bool on)
{
	if (on)
		return pmic_bus_setbits(AXP818_OUTPUT_CTRL2,
					AXP818_OUTPUT_CTRL2_SW_EN);

	return pmic_bus_clrbits(AXP818_OUTPUT_CTRL2,
				AXP818_OUTPUT_CTRL2_SW_EN);
}

int axp_gpio0_enable_ldo_set_voltage(u32 mV)
{
	int ret;
	u8 val;

	if (mV == 0)
		return 0; // do nothing ATM, which is fine since LDO is disabled
			  // by default
	else if (mV <= 700)
		val = 0;
	else if (mV >= 3300)
		val = 0x1a;
	else
		val = (mV - 700) / 100;

	ret = pmic_bus_write(AXP_GPIO0_LDO_CTRL, val);
	if (ret)
		return ret;

	return pmic_bus_write(AXP_GPIO0_CTRL, AXP_GPIO_CTRL_LDO_ON);
}

int axp_init(void)
{
	u8 axp_chip_id;
	int ret;

	ret = pmic_bus_init();
	if (ret)
		return ret;

	ret = pmic_bus_read(AXP818_CHIP_ID, &axp_chip_id);
	if (ret)
		return ret;

	if (!(axp_chip_id == 0x51))
		return -ENODEV;
	else
		return ret;

	return 0;
}

int do_poweroff(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	pmic_bus_write(AXP818_SHUTDOWN, AXP818_SHUTDOWN_POWEROFF);

	/* infinite loop during shutdown */
	while (1) {}

	/* not reached */
	return 0;
}

// battery/charger related functions

int axp_is_charging(bool* charging)
{
	int ret;
	u8 cs;

	ret = pmic_bus_read(AXP_CHARGER_STATUS, &cs);
	if (ret)
		return ret;

	*charging = (cs & AXP_CHARGER_STATUS_CHARGING);
	return 0;
}

int axp_is_vbus_present(bool* vbus)
{
	int ret;
	u8 ps;

	ret = pmic_bus_read(AXP_POWER_STATUS, &ps);
	if (ret)
		return ret;

	*vbus = (ps & AXP_POWER_STATUS_VBUS_PRESENT) &&
		(ps & AXP_POWER_STATUS_VBUS_VALID);
	return 0;
}

int axp_is_battery_present(bool* vbus)
{
	int ret;
	u8 cs;

	ret = pmic_bus_read(AXP_CHARGER_STATUS, &cs);
	if (ret)
		return ret;

	*vbus = (cs & AXP_CHARGER_STATUS_BAT_PRESENT_VALID) &&
		(cs & AXP_CHARGER_STATUS_BAT_PRESENT);
	return 0;
}

int axp_clear_startup_reason(void)
{
	return pmic_bus_write(AXP_POWER_UP_DOWN_REASON, 0xff);
}

static int axp_read_adc(u8 addr_msb, u8 addr_lsb, u32* raw)
{
	u8 val;
	int ret;

	ret = pmic_bus_read(addr_lsb, &val);
	if (ret)
		return ret;

	*raw = val & 0xf;

	ret = pmic_bus_read(addr_msb, &val);
	if (ret)
		return ret;

	*raw |= (u32)val << 4;
	return 0;
}

int axp_get_battery_voltage(int* uV)
{
	u32 raw;
	int ret;

	ret = axp_read_adc(AXP_AD_BAT_VOLTAGE_MSB8, AXP_AD_BAT_VOLTAGE_LSB4,
			   &raw);
	if (ret)
		return ret;

	*uV = (raw) * 1100;
	return 0;
}

int axp_get_battery_current(int* uA)
{
	u32 raw;
	int ret;
	bool charging;

	ret = axp_is_charging(&charging);
	if (ret)
		return ret;

	if (charging)
		ret = axp_read_adc(AXP_AD_BAT_CHG_CURRENT_MSB8,
				   AXP_AD_BAT_CHG_CURRENT_LSB4, &raw);
	else
		ret = axp_read_adc(AXP_AD_BAT_DIS_CURRENT_MSB8,
				   AXP_AD_BAT_DIS_CURRENT_LSB4, &raw);
	if (ret)
		return ret;

	*uA = raw * 1000;
	return 0;
}

int axp_battery_get_capacity(int* capacity)
{
	int ret;
	u8 val;

	ret = pmic_bus_read(AXP_BATTERY_CAPACITY, &val);
	if (ret)
		return ret;

	if (!(val & 0x80))
		return -EINVAL;

	*capacity = val & 0x7f;
	return 0;
}

struct {
	u8 vbus_i;
	int mA;
} vbus_currents[] = {
	{ AXP_CHARGER_CTRL3_VBUS_I_100,   100 },
	{ AXP_CHARGER_CTRL3_VBUS_I_500,   500 },
	{ AXP_CHARGER_CTRL3_VBUS_I_900,   900 },
	{ AXP_CHARGER_CTRL3_VBUS_I_1500, 1500 },
	{ AXP_CHARGER_CTRL3_VBUS_I_2000, 2000 },
	{ AXP_CHARGER_CTRL3_VBUS_I_2500, 2500 },
	{ AXP_CHARGER_CTRL3_VBUS_I_3000, 3000 },
	{ AXP_CHARGER_CTRL3_VBUS_I_3500, 3500 },
	{ AXP_CHARGER_CTRL3_VBUS_I_4000, 4000 },
};

int axp_usb_get_max_current(int* mA)
{
	int i, ret;
	u8 val;

	ret = pmic_bus_read(AXP_CHARGER_CTRL3, &val);
	if (ret)
		return ret;

	val &= AXP_CHARGER_CTRL3_VBUS_I_MASK;

	*mA = 4000;
	for (i = 0; i < ARRAY_SIZE(vbus_currents); i++) {
		if (vbus_currents[i].vbus_i == val) {
			*mA = vbus_currents[i].mA;
			break;
		}
	}

	return 0;
}

int axp_usb_set_max_current(int mA)
{
	int i, ret;
	u8 val;

	ret = pmic_bus_read(AXP_CHARGER_CTRL3, &val);
	if (ret)
		return ret;

	val &= ~AXP_CHARGER_CTRL3_VBUS_I_MASK;

	for (i = ARRAY_SIZE(vbus_currents) - 1; i >= 0; i--) {
		if (vbus_currents[i].mA <= mA || i == 0) {
			val |= vbus_currents[i].vbus_i;
			break;
		}
	}

	return pmic_bus_write(AXP_CHARGER_CTRL3, val);
}

int axp_led_set_charger_controlled(void)
{
	int ret;
	u8 val;

	ret = pmic_bus_read(AXP_CHGLED, &val);
	if (ret)
		return ret;

	val |= AXP_CHGLED_CTRL_CHARGER;

	return pmic_bus_write(AXP_CHGLED, val);
}

int axp_led_set(int mode)
{
	int ret;
	u8 val;

	ret = pmic_bus_read(AXP_CHGLED, &val);
	if (ret)
		return ret;

	val &= ~(AXP_CHGLED_CTRL_CHARGER | AXP_CHGLED_SETUP_MASK);
	switch (mode) {
		case 0:
			val |= AXP_CHGLED_SETUP_OFF;
			break;
		case 1:
			val |= AXP_CHGLED_SETUP_ON;
			break;
		case 2:
			val |= AXP_CHGLED_SETUP_BLINK_FAST;
			break;
		case 3:
			val |= AXP_CHGLED_SETUP_BLINK_SLOW;
			break;
		default:
			return -1;
	}

	return pmic_bus_write(AXP_CHGLED, val);
}

int axp_pok_set_quick(void)
{
	int ret;
	u8 val;

	ret = pmic_bus_read(AXP_POK, &val);
	if (ret)
		return ret;

	val &= ~(AXP_POK_ON_MASK | AXP_POK_OFF_MASK);
	val |= AXP_POK_ON_128MS | AXP_POK_OFF_4S;

	return pmic_bus_write(AXP_POK, val);
}

int axp_set_bc_en(void)
{
	int ret;
	u8 val;

	/* Enable USB Battery Charging specification detection */
	ret = pmic_bus_read(AXP_BC_GLOBAL, &val);
	if (ret)
		return ret;

	val |= AXP_BC_GLOBAL_EN;

	return pmic_bus_write(AXP_BC_GLOBAL, val);
}
