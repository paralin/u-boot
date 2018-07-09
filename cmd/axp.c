// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Ondrej Jirman <megous@megous.com>
 */
#include <common.h>
#include <command.h>
#include <cli_hush.h>
#include <axp_pmic.h>

static int do_axp(cmd_tbl_t *cmdtp, int flag, int argc,
		  char *const argv[])
{
	int ret = 0, val;
	bool val_bool;
	char buf[64];

	if (argc < 2)
		return CMD_RET_USAGE;

	if (!strcmp(argv[1], "dump")) {
		printf("AXP status:\n");

		ret = axp_is_charging(&val_bool);
		if (!ret)
			printf("  charging: %s\n", val_bool ? "yes" : "no");

		ret = axp_is_vbus_present(&val_bool);
		if (!ret)
			printf("  vbus_ok: %s\n", val_bool ? "yes" : "no");

		ret = axp_is_battery_present(&val_bool);
		if (!ret)
			printf("  bat_ok: %s\n", val_bool ? "yes" : "no");

		ret = axp_get_battery_voltage(&val);
		if (!ret)
			printf("  bat_voltage: %d uV\n", val);

		ret = axp_get_battery_current(&val);
		if (!ret)
			printf("  bat_current: %d uA\n", val);

		ret = axp_battery_get_capacity(&val);
		if (!ret)
			printf("  bat_capacity: %d %%\n", val);

		ret = axp_usb_get_max_current(&val);
		if (!ret)
			printf("  vbus_imax: %d mA\n", val);
	} else if (!strcmp(argv[1], "charging")) {
		ret = axp_is_charging(&val_bool);
		if (ret)
			return CMD_RET_FAILURE;

		snprintf(buf, sizeof buf, "ret=%d", !!val_bool);
		set_local_var(buf, 1);

		//int axp_usb_set_max_current(int mA);
	} else if (!strcmp(argv[1], "vbus_ok")) {
		ret = axp_is_vbus_present(&val_bool);
		if (ret)
			return CMD_RET_FAILURE;

		snprintf(buf, sizeof buf, "ret=%d", !!val_bool);
		set_local_var(buf, 1);
	} else if (!strcmp(argv[1], "bat_ok")) {
		ret = axp_is_battery_present(&val_bool);
		if (ret)
			return CMD_RET_FAILURE;

		snprintf(buf, sizeof buf, "ret=%d", !!val_bool);
		set_local_var(buf, 1);
	} else if (!strcmp(argv[1], "bat_voltage")) {
		ret = axp_get_battery_voltage(&val);
		if (ret)
			return CMD_RET_FAILURE;

		snprintf(buf, sizeof buf, "ret=%d", val);
		set_local_var(buf, 1);
	} else if (!strcmp(argv[1], "bat_current")) {
		ret = axp_get_battery_current(&val);
		if (ret)
			return CMD_RET_FAILURE;

		snprintf(buf, sizeof buf, "ret=%d", val);
		set_local_var(buf, 1);
	} else if (!strcmp(argv[1], "bat_capacity")
		   || !strcmp(argv[1], "bat_capacity5")) {
		ret = axp_battery_get_capacity(&val);
		if (ret)
			return CMD_RET_FAILURE;

		if (!strcmp(argv[1], "bat_capacity5")) {
			// round to multiple of 5
			int rem = val % 5;
			if (rem > 2)
				val += (5 - rem);
			else
				val -= rem;
		}

		snprintf(buf, sizeof buf, "ret=%d", val);
		set_local_var(buf, 1);
	} else if (!strcmp(argv[1], "vbus_imax")) {
		if (argc == 3) {
			val = simple_strtoul(argv[2], NULL, 10);
			if (val == 0)
				return CMD_RET_USAGE;

			ret = axp_usb_set_max_current(val);
			if (ret)
				return CMD_RET_FAILURE;
		} else if (argc == 2) {
			ret = axp_usb_get_max_current(&val);
			if (ret)
				return CMD_RET_FAILURE;

			snprintf(buf, sizeof buf, "ret=%d", val);
			set_local_var(buf, 1);
		} else
			return CMD_RET_USAGE;
	} else if (!strcmp(argv[1], "led_set")) {
		if (argc < 3)
			return CMD_RET_USAGE;

		if (strlen(argv[2]) != 1)
			return CMD_RET_USAGE;
		switch (argv[2][0]) {
			case '0':
				ret = axp_led_set(0);
				break;
			case '1':
				ret = axp_led_set(1);
				break;
			case 'a':
				ret = axp_led_set(2);
				break;
			case 'b':
				ret = axp_led_set(3);
				break;
			default:
				return CMD_RET_USAGE;
		}

		if (ret)
			return CMD_RET_FAILURE;
	} else if (!strcmp(argv[1], "led_chgctl")) {
		ret = axp_led_set_charger_controlled();
		if (ret)
			return CMD_RET_FAILURE;
	} else if (!strcmp(argv[1], "clr_reason")) {
		ret = axp_clear_startup_reason();
		if (ret)
			return CMD_RET_FAILURE;
	} else if (!strcmp(argv[1], "pok_fast")) {
		ret = axp_pok_set_quick();
		if (ret)
			return CMD_RET_FAILURE;
	} else if (!strcmp(argv[1], "bc_en")) {
		ret = axp_set_bc_en();
		if (ret)
			return CMD_RET_FAILURE;
	} else
		return CMD_RET_USAGE;

	return CMD_RET_SUCCESS;
}

static char axp_help_text[] =
	"dump - dump AXP device info\n"
	"axp charging - Check if battery is charging\n"
	"axp vbus_ok - Check if connected to USB charger\n"
	"axp bat_ok - Check if battery is present/OK\n"
	"axp bat_voltage - Return battery voltage in uV\n"
	"axp bat_current - Return battery current in uA\n"
	"axp bat_capacity - Return battery capacity in %\n"
	"axp bat_capacity5 - As above rounded to multiples of 5\n"
	"axp vbus_imax - Return max USB current in mA\n"
	"axp vbus_imax <mA> - Set max USB current to <mA> value\n"
	"axp led_set 1|0 - Turn charger led on/off\n"
	"axp led_chgctl - Defer LED control to the charger\n"
	// Especially useful for escaping UV lockout
	"axp clr_reason - Clear startup/shutdown reason register\n"
	"axp bc_en - Charger detection enable\n"
	"axp pok_fast - Set power on/off key to low latency mode";

U_BOOT_CMD(axp, 4, 1, do_axp, "AXP813 PMIC access", axp_help_text);
