// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 BayLibre, SAS
 * Author: Neil Armstrong <narmstrong@baylibre.com>
 */
#include <common.h>
#include <command.h>
#include <dm.h>
#include <touchpanel.h>

static int do_touch_list(cmd_tbl_t *cmdtp, int flag, int argc,
		       char *const argv[])
{
	struct udevice *dev;
	int ret;

	ret = uclass_first_device_err(UCLASS_TOUCHPANEL, &dev);
	if (ret) {
		printf("No available touchpanel device\n");
		return CMD_RET_FAILURE;
	}

	do {
		printf("- %s\n", dev->name);

		ret = uclass_next_device(&dev);
		if (ret)
			return CMD_RET_FAILURE;
	} while (dev);

	return CMD_RET_SUCCESS;
}

static int do_touch_info(cmd_tbl_t *cmdtp, int flag, int argc,
		       char *const argv[])
{
	struct touchpanel_priv *uc_priv;
	struct udevice *dev;
	int ret;

	if (argc < 2)
		return CMD_RET_USAGE;

	ret = uclass_get_device_by_name(UCLASS_TOUCHPANEL, argv[1], &dev);
	if (ret) {
		printf("Unknown touchpanel device %s\n", argv[1]);
		return CMD_RET_FAILURE;
	}

	uc_priv = dev_get_uclass_priv(dev);

	printf("Touchpanel Device '%s' :\n", argv[1]);
	printf("size_x: %d\n", uc_priv->size_x);
	printf("size_y: %d\n", uc_priv->size_y);

	return CMD_RET_SUCCESS;
}

static int do_touch_get(cmd_tbl_t *cmdtp, int flag, int argc,
			char *const argv[])
{
	//struct touchpanel_priv *uc_priv;
	struct touchpanel_touch touches[10];
	struct udevice *dev;
	int ret, i;

	if (argc < 2)
		return CMD_RET_USAGE;

	ret = uclass_get_device_by_name(UCLASS_TOUCHPANEL, argv[1], &dev);
	if (ret) {
		printf("Unknown touchpanel device %s\n", argv[1]);
		return CMD_RET_FAILURE;
	}

	//uc_priv = dev_get_uclass_priv(dev);

	printf("Touchpanel Device '%s' :\n", argv[1]);

	ret = touchpanel_start(dev);
	if (ret < 0) {
		printf("Failed to start %s, err=%d\n", argv[1], ret);
		return CMD_RET_FAILURE;
	}

	ret = touchpanel_get_touches(dev, touches, ARRAY_SIZE(touches));
	if (ret < 0) {
		printf("Failed to get touches from %s, err=%d\n", argv[1], ret);
		return CMD_RET_FAILURE;
	}

        for (i = 0; i < ret; i++) {
		printf("touch: id=%d x=%d y=%d\n", touches[i].id, touches[i].x,
		       touches[i].y);
	}

	ret = touchpanel_stop(dev);
	if (ret < 0) {
		printf("Failed to stop %s, err=%d\n", argv[1], ret);
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_touch_sub[] = {
	U_BOOT_CMD_MKENT(list, 1, 1, do_touch_list, "", ""),
	U_BOOT_CMD_MKENT(info, 2, 1, do_touch_info, "", ""),
	U_BOOT_CMD_MKENT(get, 2, 1, do_touch_get, "", ""),
};

static int do_touch(cmd_tbl_t *cmdtp, int flag, int argc,
		  char *const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading 'touch' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_touch_sub[0], ARRAY_SIZE(cmd_touch_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

static char touch_help_text[] =
	"list - list touchpanel devices\n"
	"touch info <name> - Get touchpanel device info\n"
	"touch get <name> - Get touches";

U_BOOT_CMD(touch, 4, 1, do_touch, "Touchpanel sub-system", touch_help_text);
