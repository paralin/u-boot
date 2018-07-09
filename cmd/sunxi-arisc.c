/*
 * Written by Ondrej Jirman <megous@megous.com>, 2018
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <watchdog.h>

#define R_CPUCFG_BASE	0x01f01c00

static void reset_cpus(int stop)
{
	uint32_t cpu_cfg;
	
	cpu_cfg = readl(R_CPUCFG_BASE);
	cpu_cfg &= ~(1uL);
	writel(cpu_cfg, R_CPUCFG_BASE);

	mdelay(1);

	if (!stop) {
		cpu_cfg |= 1;
		writel(cpu_cfg, R_CPUCFG_BASE);
	}
}

static int do_arisc_start(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	reset_cpus(0);
	return 0;
}

static int do_arisc_stop(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	reset_cpus(1);
	return 0;
}

static int do_arisc_takeover(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	reset_cpus(0);

	while (1) {
		WATCHDOG_RESET();
		mdelay(10);
	}

	return 0;
}

U_BOOT_CMD(
	arisc_start, 3, 1, do_arisc_start,
	"reset arisc comprocessor and de-assert the reset line",
	""
);

U_BOOT_CMD(
	arisc_stop, 3, 1, do_arisc_stop,
	"reset arisc comprocessor and keep it reset",
	""
);

U_BOOT_CMD(
	arisc_takeover, 3, 1, do_arisc_takeover,
	"reset arisc comprocessor and de-assert the reset line, stop u-boot",
	""
);
