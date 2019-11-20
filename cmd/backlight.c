// SPDX-License-Identifier: GPL-2.0+
#include <common.h>
#include <command.h>
#include <dm.h>
#include <backlight.h>

static int do_backlight(cmd_tbl_t *cmdtp, int flag, int argc,
			char *const argv[])
{
	struct udevice *backlight;
	int ret;

	ret = uclass_get_device(UCLASS_PANEL_BACKLIGHT, 0, &backlight);
	if (!ret)
		backlight_enable(backlight);

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(backlight_on, 1, 1, do_backlight, "Backlight", " - enable backlightpanel devices");
