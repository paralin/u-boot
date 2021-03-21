/*
 * (C) Copyright 2020 Hardkernel Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <adc.h>

#define check_range(min,max,val) (val > 0 && val > min && val < max ? 1 : 0)

int do_hwrev(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int hwrev_adc;

	if (adc_channel_single_shot("saradc@ff288000", 0, &hwrev_adc)) {
		printf("board hw rev failed\n");
		return CMD_RET_FAILURE;
	}

	/* GO2 rev 1.1 */
	if (check_range(655, 695, hwrev_adc)) {
		env_set("hwrev", "v11");
		env_set("dtb_name", "/boot/rk3326-odroid-go2-v11.dtb");
	}
	/* GO2 rev 1.0 */
	else if (check_range(816, 896, hwrev_adc)) {
		env_set("hwrev", "v10");
		env_set("dtb_name", "/boot/rk3326-odroid-go2.dtb");
	}
	/* GO3 rev 1.0 */
	else if (check_range(40, 126, hwrev_adc)) {
		env_set("hwrev", "v10-go3");
		env_set("dtb_name", "/boot/rk3326-odroid-go3.dtb");
	}
	/* engineer samples */
	else {
		env_set("hwrev", "v00");
		env_set("dtb_name", "/boot/rk3326-odroid-go2.dtb");
	}

	env_set("bootargs", "console=ttyS2,115200 root=/dev/mmcblk0p1 rootfstype=f2fs console=tty0 fbcon=rotate:3 rw quiet");

	printf("adc0 (hw rev) %d\n", hwrev_adc);

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	hwrev, 1, 1, do_hwrev,
	"check hw revision of OGA",
	""
);
