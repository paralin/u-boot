/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2014 Chen-Yu Tsai <wens@csie.org>
 *
 * Configuration settings for the Allwinner A23 (sun8i) CPU
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * A23 specific configuration
 */

#include <asm/arch/cpu.h>

#ifdef SUNXI_SRAM_A2_SIZE
#define SUNXI_RESUME_BASE		(CONFIG_ARMV7_SECURE_BASE + \
					 CONFIG_ARMV7_SECURE_MAX_SIZE)
#define SUNXI_RESUME_SIZE		1024
#endif

/*
 * Include common sunxi configuration where most the settings are
 */
#include <configs/sunxi-common.h>

#endif /* __CONFIG_H */
