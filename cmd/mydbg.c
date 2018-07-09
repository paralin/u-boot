// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018 Ondrej Jirman <megous@megous.com>
 */
#include <common.h>
#include <command.h>
#include <cli_hush.h>
#include <axp_pmic.h>
#include <asm/arch/pmic_bus.h>
#include <asm/io.h>

/* This is for A83T */

#define PIO_BASE 0x01C20800
#define TCON0_BASE 0x01c0C000
#define CCU_BASE 0x01C20000
#define DE2_BASE 0x01000000
#define DE2_MUX0_BASE (DE2_BASE + 0x100000)
#define DE2_MUX1_BASE (DE2_BASE + 0x200000)
#define PIO(n, a) readl(PIO_BASE + ((n) * 0x24 + a))
#define PWM_BASE 0x01C21400
#define TWI0_BASE 0x01c2ac00
#define MMC0_BASE 0x01c0f000
#define MMC1_BASE 0x01c10000
#define MMC2_BASE 0x01c11000

static void dump_regs(void)
{
	int i;
/*
	printf("PWM:\n");
	for (i = 0; i < 2; i++) {
		printf("0x%08x : %08x\n", PWM_BASE + i*4, readl(PWM_BASE + i*4));
	}

	printf("TCON0:\n");
	for (i = 0; i <= 0x100; i++) {
		printf("0x%08x : %08x\n", TCON0_BASE + i*4, readl(TCON0_BASE + i*4));
	}

	printf("DE2:\n");
	for (i = 0; i <= 0x100; i++) {
		printf("0x%08x : %08x\n", DE2_BASE + i*4, readl(DE2_BASE + i*4));
	}

	printf("DE2 MIX:\n");
	for (i = 0; i <= 0x100; i++) {
		printf("0x%08x : %08x\n", DE2_BASE + 0x100000 + i*4, readl(DE2_BASE + 0x100000 + i*4));
	}

	printf("TWI0:\n");
	for (i = 0; i <= 32; i++) {
		printf("0x%08x : %08x\n", TWI0_BASE + i*4, readl(TWI0_BASE + i*4));
	}
*/
	printf("CCU:\n");
//	for (i = 0; i <= 0x100; i++) {
	for (i = 34; i <= 36; i++) {
		printf("0x%08x : %08x\n", CCU_BASE + i*4, readl(CCU_BASE + i*4));
	}

	printf("MMC0:\n");
	for (i = 0; i <= 67; i++) {
		printf("0x%08x : %08x\n", MMC0_BASE + i*4, readl(MMC0_BASE + i*4));
	}

	printf("MMC1:\n");
	for (i = 0; i <= 67; i++) {
		printf("0x%08x : %08x\n", MMC1_BASE + i*4, readl(MMC1_BASE + i*4));
	}

	printf("MMC2:\n");
	for (i = 0; i <= 67; i++) {
		printf("0x%08x : %08x\n", MMC2_BASE + i*4, readl(MMC2_BASE + i*4));
	}

/*
	for (i = 1; i <= 7; i++) {
		printf("P%c_CFG0 = %08x\n", 'A' + i, PIO(i, 0x00));
		printf("P%c_CFG1 = %08x\n", 'A' + i, PIO(i, 0x04));
		printf("P%c_CFG2 = %08x\n", 'A' + i, PIO(i, 0x08));
		printf("P%c_CFG3 = %08x\n", 'A' + i, PIO(i, 0x0c));
		printf("P%c_DAT  = %08x\n", 'A' + i, PIO(i, 0x10));
		printf("P%c_DRV0 = %08x\n", 'A' + i, PIO(i, 0x14));
		printf("P%c_DRV1 = %08x\n", 'A' + i, PIO(i, 0x18));
		printf("P%c_PUL0 = %08x\n", 'A' + i, PIO(i, 0x1c));
		printf("P%c_PUL1 = %08x\n", 'A' + i, PIO(i, 0x20));
	}

	printf("PMIC:\n");
	for (i = 0; i < 256; i++) {
		u8 val;

		int ret = pmic_bus_read(i, &val);
		if (!ret)
			printf("%02x = %02x\n", i, (unsigned int)val);
	}
*/
}

static int do_mydbg(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	dump_regs();

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(mydbg, 3, 1, do_mydbg, "mydbg", "mydbg - show some debug values");
