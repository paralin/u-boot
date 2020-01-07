/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2016 Rockchip Electronics Co., Ltd
 * Copyright (C) 2020 Peter Robinson <pbrobinson at gmail.com>
 */

#ifndef __PINEBOOK_PRO_RK3399_H
#define __PINEBOOK_PRO_RK3399_H

#define ROCKCHIP_DEVICE_SETTINGS \
		"beep_boop=led green:power on; led red:standby on; sleep 0.1; led green:power off; sleep 0.1; led green:power on; sleep 0.1; led red:standby off; sleep 0.1; led red:standby on\0" \
		"bootcmd=run beep_boop; run distro_bootcmd\0" \
		"stdin=serial,usbkbd\0" \
		"stdout=serial,vidconsole\0" \
		"stderr=serial,vidconsole\0"

#include <configs/rk3399_common.h>

#define SDRAM_BANK_SIZE			(2UL << 30)

#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS     2

#endif
