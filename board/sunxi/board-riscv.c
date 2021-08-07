#include <common.h>
#include <cpu.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;

void *board_fdt_blob_setup(int *err)
{
	*err = 0;

	return (void *)(ulong)gd->arch.firmware_fdt_addr;
}

int board_init(void)
{
	/* https://lore.kernel.org/u-boot/31587574-4cd1-02da-9761-0134ac82b94b@sholland.org/ */
	return cpu_probe_all();
}

uint32_t spl_boot_device(void)
{
	return BOOT_DEVICE_MMC1;
}
