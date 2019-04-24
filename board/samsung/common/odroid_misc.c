/*
 * Odroid Board setup for EXYNOS5 based board
 *
 * Copyright (C) 2017 Hardkernel Co.,Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/arch/board.h>
#include <asm/arch/dwmmc.h>
#include <asm/arch/mmc.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/power.h>
#include <asm/arch/system.h>
#include <asm/arch/sromc.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <samsung/misc.h>
#include <samsung/odroid_misc.h>
#include <errno.h>
#include <version.h>
#include <malloc.h>
#include <memalign.h>
#include <linux/sizes.h>
#include <linux/input.h>
#include <power/pmic.h>
#include <power/regulator.h>
#include <power/s2mps11.h>
#include <dm.h>
#include <adc.h>
#include <mmc.h>

DECLARE_GLOBAL_DATA_PTR;

/* ODROID Debug Message */
/*
#define	ODROID_MISC_DEBUG
*/

/*
	Default Partition Info for Android system
*/
static struct partition_info gPartInfo[PART_MAX] = {
	[PART_FWBL1] = {
		.name 	   = "fwbl1",
		.blk_start = PART_BL1_ST_BLK,
		.size 	   = PART_SIZE_BL1,
		.raw_en    = 1,
	},
	[PART_BL2] = {
		.name 	   = "bl2",
		.blk_start = PART_BL2_ST_BLK,
		.size 	   = PART_SIZE_BL2,
		.raw_en    = 1,
	},
	[PART_BOOTLOADER] = {
		.name 	   = "bootloader",
		.blk_start = PART_UBOOT_ST_BLK,
		.size 	   = PART_SIZE_UBOOT,
		.raw_en    = 1,
	},
	[PART_TZSW] = {
		.name 	   = "tzsw",
		.blk_start = PART_TZSW_ST_BLK,
		.size 	   = PART_SIZE_TZSW,
		.raw_en    = 1,
	},
	[PART_ENV] = {
		.name 	   = "env",
		.blk_start = PART_ENV_ST_BLK,
		.size 	   = PART_SIZE_ENV,
		.raw_en    = 0,
	},
	[PART_KERENEL] = {
		.name 	   = "kernel",
		.blk_start = PART_KERNEL_ST_BLK,
		.size 	   = PART_SIZE_KERNEL,
		.raw_en    = 0,
	},
	[PART_SYSTEM] = {
		.name 	   = "system",
		.blk_start = 0,
		.size 	   = 0,
		.raw_en    = 0,
	},
	[PART_USERDATA] = {
		.name 	   = "userdata",
		.blk_start = 0,
		.size 	   = 0,
		.raw_en    = 0,
	},
	[PART_CACHE] = {
		.name 	   = "cache",
		.blk_start = 0,
		.size 	   = 0,
		.raw_en    = 0,
	},
};

/*---------------------------------------------------------------------------*/
/* from cmd/mmc.c */
extern int get_mmc_part_info(char *device_name, int part_num, uint *block_start,
	uint *block_count, uchar *part_Id);

/*---------------------------------------------------------------------------*/
static void odroid_print_part_info(char *dev_no)
{
	int i;

	printf("\n*** Partition Information for Andorid ***");
	printf("\nControl Device ID : %s", dev_no);
	printf("\npNo\tStart Block\tpSize(bytes)\tpName");
	for (i = 0; i < PART_MAX; i++) {
		printf("\n %d \t0x%08x\t0x%08x\t%s",
			i,
			gPartInfo[i].blk_start,
			gPartInfo[i].size,
			gPartInfo[i].name);
	}
	printf("\n\n");
}

/*---------------------------------------------------------------------------*/
int odroid_get_partition_info(const char *ptn, struct partition_info *pinfo)
{
	int i;

	for (i = 0; i < PART_MAX; i++) {
		if (!strncmp(gPartInfo[i].name, ptn, strlen(ptn)))
			break;
	}
	if (PART_MAX == i)
		return	-1;

	memcpy((void *)pinfo, (void *)&gPartInfo[i],
		sizeof(struct partition_info));
	return	0;
}

/*---------------------------------------------------------------------------*/
/*
	Partition information setup for Android fastboot.

	Partition 1 = fat partition
	Partition 2 - system partition
	Partition 3 - userdata partition
	Partition 4 - cache partition
*/
/*---------------------------------------------------------------------------*/
int odroid_partition_setup(char *dev_no)
{
	unsigned int blk_st, blk_cnt;
	unsigned char pid, i;

	for (i = 0; i < 3; i++)	{
		if (get_mmc_part_info (dev_no, i + 2, &blk_st, &blk_cnt, &pid))
			goto err;
		if (pid != 0x83)
			goto err;
		gPartInfo[PART_SYSTEM+i].blk_start = blk_st;
		gPartInfo[PART_SYSTEM+i].size = (blk_cnt * MOVI_BLK_SIZE);
	}

	odroid_print_part_info(dev_no);
	return	0;
err:
	printf( "\n****************************\n" \
		"\n***      Warning!!!      ***\n" \
		"\n****************************\n" \
		"\This is not an Android Partition device!" \
		"\nIf you want Android partitioning," \
		"use fdisk command befor fastboot command.\n\n");
	return	-1;
}

/*---------------------------------------------------------------------------*/
void odroid_led_ctrl(int gpio, int status)
{
	gpio_set_value(gpio, status);
}

/*---------------------------------------------------------------------------*/
static void odroid_pmic_deinit(void)
{
	struct udevice *dev;

	if (pmic_get("s2mps11", &dev))	{
		printf("%s : s2mps11 control error!\n", __func__);
		return;
	}
	/* Master Reset Enable */
	pmic_reg_write(dev, S2MPS11_REG_CTRL1, 0x0);
}

/*---------------------------------------------------------------------------*/
static void odroid_pmic_init(void)
{
	struct udevice *dev;

	if (pmic_get("s2mps11", &dev))	{
		printf("%s : s2mps11 control error!\n", __func__);
		return;
	}

	/* LDO9 : USB 3.0 3.3V */
	pmic_reg_write(dev, S2MPS11_REG_L9CTRL, 0xF2);

	/* LDO15, LDO17 : ETH 3.3V */
	pmic_reg_write(dev, S2MPS11_REG_L15CTRL, 0xF2);
	pmic_reg_write(dev, S2MPS11_REG_L17CTRL, 0xF2);

	/* LDO13, LDO19 : MMC 3.3V */
	pmic_reg_write(dev, S2MPS11_REG_L13CTRL, 0xF2);
	pmic_reg_write(dev, S2MPS11_REG_L19CTRL, 0xF2);

	/* BUCK10 : eMMC 2.85V */
	pmic_reg_write(dev, S2MPS11_REG_B10CTRL2, 0xA8);

	/* Master Reset Enable */
	pmic_reg_write(dev, 0x0c, 0x10);

#if defined(ODROID_MISC_DEBUG)
	/* debug message */
	printf("S2MPS11_REG_L9CTRL = 0x%02X\n",
		pmic_reg_read(dev, S2MPS11_REG_L9CTRL));
	printf("S2MPS11_REG_L15CTRL = 0x%02X\n",
		pmic_reg_read(dev, S2MPS11_REG_L15CTRL));
	printf("S2MPS11_REG_L17CTRL = 0x%02X\n",
		pmic_reg_read(dev, S2MPS11_REG_L17CTRL));
	printf("S2MPS11_REG_L13CTRL = 0x%02X\n",
		pmic_reg_read(dev, S2MPS11_REG_L13CTRL));
	printf("S2MPS11_REG_L19CTRL = 0x%02X\n",
		pmic_reg_read(dev, S2MPS11_REG_L19CTRL));
	printf("S2MPS11_REG_B10CTRL2 = 0x%02X\n",
		pmic_reg_read(dev, S2MPS11_REG_B10CTRL2));
#endif
}

/*---------------------------------------------------------------------------*/
static void odroid_led_deinit(void)
{
	odroid_led_ctrl(GPIO_LED_R, 0);
	odroid_led_ctrl(GPIO_LED_G, 0);
	odroid_led_ctrl(GPIO_LED_B, 0);
	gpio_free(GPIO_LED_R);
	gpio_free(GPIO_LED_G);
	gpio_free(GPIO_LED_B);
}

/*---------------------------------------------------------------------------*/
static void odroid_led_init(void)
{
	if (gpio_request(GPIO_LED_R, "LED-R")) 
		goto	err;
	odroid_led_ctrl(GPIO_LED_R, 0);

	if (gpio_request(GPIO_LED_G, "LED-G"))
		goto	err;
	odroid_led_ctrl(GPIO_LED_G, 0);

	/* Default On */
	if (gpio_request(GPIO_LED_B, "LED-B"))
		goto	err;
	odroid_led_ctrl(GPIO_LED_B, 1);

	return;
err:
	printf("%s : GPIO Control error!\n", __func__);
}

/*---------------------------------------------------------------------------*/
static void odroid_gpio_deinit(void)
{
	gpio_direction_output(GPIO_FAN_CTL, 0);
	gpio_direction_output(GPIO_LCD_PWM, 0);

	gpio_free(GPIO_POWER_BT);
	gpio_free(GPIO_FAN_CTL);
	gpio_free(GPIO_LCD_PWM);
}

/*---------------------------------------------------------------------------*/
static void odroid_gpio_init(void)
{
	/* Power control button pin */
	if (gpio_request(GPIO_POWER_BT, "Power BT"))
		goto err;
	gpio_set_pull(GPIO_POWER_BT, S5P_GPIO_PULL_NONE);
	gpio_direction_input(GPIO_POWER_BT);

	/* FAN Full Enable */
	if (gpio_request(GPIO_FAN_CTL, "FAN Ctrl"))
		goto err;
	gpio_set_pull(GPIO_FAN_CTL, S5P_GPIO_PULL_NONE);
	gpio_direction_output(GPIO_FAN_CTL, 1);

	/* LCD PWM Port High */
	if (gpio_request(GPIO_LCD_PWM, "LCD PWM"))
		goto err;
	gpio_set_pull(GPIO_LCD_PWM, S5P_GPIO_PULL_NONE);
	gpio_direction_output(GPIO_LCD_PWM, 1);

	return;
err:
	printf("%s : GPIO Control error!\n", __func__);
}

/*---------------------------------------------------------------------------*/
static int filecheck(const char *fname)
{
	char	cmd[64];
	unsigned long	filesize = 0;

	/* env variable init */
	setenv("filesize", "0");

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(cmd, "fatload mmc 0:1 40008000 update/%s", fname);
	run_command(cmd, 0);

	/* file size check */
	if ((filesize = getenv_ulong("filesize", 16, 0)))
		return  1;

	printf("ERROR! update/%s File Not Found!! filesize = 0\n", fname);

	/* error */
	return  0;
}

/*---------------------------------------------------------------------------*/
static void update_image(const char *ptn)
{
	char	cmd[64];

	memset(cmd, 0x00, sizeof(cmd));
	sprintf(cmd, "fastboot flash %s 40008000 0", ptn);
	run_command(cmd, 0);
}

/*---------------------------------------------------------------------------*/
static void update_raw_image(const char *ptn)
{
	struct exynos5_power *pmu =
		(struct exynos5_power *)samsung_get_base_power;
	int OmPin;

	OmPin = pmu->inform3;

	if (!strncmp(ptn, "kernel", sizeof("kernel")))
		run_command("movi w k 0 40008000", 0);
	else {
		if ((OmPin == BOOT_EMMC_4_4) || (OmPin == BOOT_EMMC)) {
			run_command("emmc open 0", 0);
			if (!strncmp(ptn, "bootloader", sizeof("bootloader")))
				run_command("movi w z u 0 40008000", 0);
			if (!strncmp(ptn, "bl1", sizeof("bl1")))
				run_command("movi w z f 0 40008000", 0);
			if (!strncmp(ptn, "bl2", sizeof("bl2")))
				run_command("movi w z b 0 40008000", 0);
			if (!strncmp(ptn, "tzsw", sizeof("tzsw")))
				run_command("movi w z t 0 40008000", 0);
			run_command("emmc close 0", 0);
		} else {
			if (!strncmp(ptn, "bootloader", sizeof("bootloader")))
				run_command("movi w u 0 40008000", 0);
			if (!strncmp(ptn, "bl1", sizeof("bl1")))
				run_command("movi w f 0 40008000", 0);
			if (!strncmp(ptn, "bl2", sizeof("bl2")))
				run_command("movi w b 0 40008000", 0);
			if (!strncmp(ptn, "tzsw", sizeof("tzsw")))
				run_command("movi w t 0 40008000", 0);
		}
	}
}

/*---------------------------------------------------------------------------*/
/* firmware update check */
static void odroid_fw_update(unsigned int option)
{
	odroid_led_ctrl(GPIO_LED_B, 1);

	if (filecheck("system.img"))
		update_image("system");
	if (filecheck("cache.img"))
		update_image("cache");

	if (option & OPTION_ERASE_USERDATA) {
		if (filecheck("userdata.img"))
			update_image("userdata");
	}
	if (filecheck("zImage"))
		update_raw_image("kernel");
	else if (filecheck("zImage-dtb"))
		update_raw_image("kernel");

	if (option & OPTION_UPDATE_UBOOT) {
		if(filecheck("u-boot.bin"))
			update_raw_image("bootloader");
		if(filecheck("bl1.bin"))
			update_raw_image("bl1");
		if(filecheck("bl2.bin"))
			update_raw_image("bl2");
		if(filecheck("tzsw.bin"))
			update_raw_image("tzsw");
	}

	if (option & OPTION_ERASE_ENV)
		run_command(UBOOT_ENV_ERASE, 0);

	if (option & OPTION_ERASE_FAT)
		run_command("fatformat mmc 0:1", 0);

	odroid_led_ctrl(GPIO_LED_B, 1);
}

/*---------------------------------------------------------------------------*/
static void odroid_magic_cmd_check(void)
{
	struct exynos5_power *pmu =
		(struct exynos5_power *)samsung_get_base_power();

	unsigned int	cmd, option;

	cmd	= (pmu->sysip_dat0 & 0xFFFF);
	option	= (pmu->inform0	   & 0xFFFF);

#if defined(ODROID_MISC_DEBUG)
	printf("pmu->sysip = 0x%08x, pmu->inform0 = 0x%08x\n",
		cmd, option);
#endif
	pmu->sysip_dat0 = 0;	pmu->inform0 = 0;

	switch(cmd) {
	case	FASTBOOT_MAGIC_REBOOT_CMD:
		run_command("fastboot", 0);
		break;
	case	FASTBOOT_MAGIC_UPDATE_CMD:
		odroid_fw_update(option);
		run_command("reset", 0);
		break;
	}
}

/*---------------------------------------------------------------------------*/
/*
	ODROID XU3/XU3-Lite/XU4 Hardware Init.
	call from board/samsung/common/board.c
*/
/*---------------------------------------------------------------------------*/
void odroid_misc_init(void)
{
	/* Default LDO value setup */
	odroid_pmic_init();
	odroid_gpio_init();
	odroid_led_init();

	/* check Android Image update or fastboot Magic command */
	odroid_magic_cmd_check();
}

/*---------------------------------------------------------------------------*/
void odroid_misc_deinit(void)
{
	odroid_led_deinit();
	odroid_gpio_deinit();
	odroid_pmic_deinit();
}

/*---------------------------------------------------------------------------*/
void odroid_power_off(void)
{
	struct exynos5_power *power =
		(struct exynos5_power *)samsung_get_base_power();

	printf("%s\n", __func__);
	odroid_misc_deinit();

	power->ps_hold_control = 0x5200;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
