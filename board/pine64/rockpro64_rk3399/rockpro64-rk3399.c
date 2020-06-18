// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2019 Vasily Khoruzhick <anarsoul@gmail.com>
 */

#include <common.h>
#include <dm.h>
#include <fdt_support.h>
#include <jffs2/load_kernel.h>
#include <mtd_node.h>
#include <init.h>
#include <syscon.h>
#include <asm/io.h>
#include <asm/arch-rockchip/clock.h>
#include <asm/arch-rockchip/grf_rk3399.h>
#include <asm/arch-rockchip/hardware.h>
#include <asm/arch-rockchip/misc.h>

#define GRF_IO_VSEL_BT565_SHIFT 0
#define PMUGRF_CON0_VSEL_SHIFT 8

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, struct bd_info *bd)
{
#ifdef CONFIG_FDT_FIXUP_PARTITIONS
	int ret, noff;
	static const struct node_info nodes[] = {
		{ "jedec,spi-nor", MTD_DEV_TYPE_NOR, },
	};

	noff = fdt_node_offset_by_compatible(blob, -1, nodes[0].compat);
	if (noff != -FDT_ERR_NOTFOUND) {
		ret = fdt_setprop_u32(blob, noff, "#size-cells", 1);
		if (ret)
			return ret;
		ret = fdt_setprop_u32(blob, noff, "#address-cells", 1);
		if (ret)
			return ret;
	} else {
		return -ENODEV;
	}

	fdt_fixup_mtdparts(blob, nodes, ARRAY_SIZE(nodes));
#endif
	return 0;
}
#endif

#ifdef CONFIG_MISC_INIT_R
static void setup_iodomain(void)
{
	struct rk3399_grf_regs *grf =
	    syscon_get_first_range(ROCKCHIP_SYSCON_GRF);
	struct rk3399_pmugrf_regs *pmugrf =
	    syscon_get_first_range(ROCKCHIP_SYSCON_PMUGRF);

	/* BT565 is in 1.8v domain */
	rk_setreg(&grf->io_vsel, 1 << GRF_IO_VSEL_BT565_SHIFT);

	/* Set GPIO1 1.8v/3.0v source select to PMU1830_VOL */
	rk_setreg(&pmugrf->soc_con0, 1 << PMUGRF_CON0_VSEL_SHIFT);
}

int misc_init_r(void)
{
	const u32 cpuid_offset = 0x7;
	const u32 cpuid_length = 0x10;
	u8 cpuid[cpuid_length];
	int ret;

	setup_iodomain();

	ret = rockchip_cpuid_from_efuse(cpuid_offset, cpuid_length, cpuid);
	if (ret)
		return ret;

	ret = rockchip_cpuid_set(cpuid, cpuid_length);
	if (ret)
		return ret;

	ret = rockchip_setup_macaddr();

	return ret;
}

#endif
