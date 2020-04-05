ifeq (rock64,$(BOARD_TARGET))

UBOOT_DEFCONFIG ?= rock64-rk3328_defconfig
BL31 ?= tmp/atf/rk3328/22d12c4148c373932a7a81e5d1c59a767e143ac2.elf
BOARD_CHIP ?= rk3328
ifneq (,$(FLASH_SPI))
LOADER_BIN ?= tmp/rkbin/rk33/rk3328_loader_v1.08.244_for_spi_nor_build_Aug_7_2017.bin
else
LOADER_BIN ?= tmp/rkbin/rk33/rk3328_loader_ddr333_v1.08.244.bin
endif
SPL_FLASH_OFFSET ?= 64
UBOOT_FLASH_OFFSET ?= 512
IMAGES ?= flash-spi erase-spi
FLASH_LOADER ?= rksd_loader
LOADERS ?= rksd_loader

else ifeq (rockpro64,$(BOARD_TARGET))

UBOOT_DEFCONFIG ?= rockpro64-rk3399_defconfig
BL31 ?= tmp/atf/rk3399/22d12c4148c373932a7a81e5d1c59a767e143ac2.elf
BOARD_CHIP ?= rk3399
ifneq (,$(FLASH_SPI))
LOADER_BIN ?= tmp/rkbin/rk33/rk3399_loader_spinor_v1.24.114.bin
SPL_FLASH_OFFSET ?= 0
UBOOT_FLASH_OFFSET ?= 1024
FLASH_LOADER ?= rkspi_loader
else
LOADER_BIN ?= tmp/rkbin/rk33/rk3399_loader_v1.24.124.bin
SPL_FLASH_OFFSET ?= 64
UBOOT_FLASH_OFFSET ?= 512
FLASH_LOADER ?= rksd_loader
endif
IMAGES ?= flash-spi erase-spi
LOADERS ?= rksd_loader rkspi_loader

else ifeq (rockpi4b,$(BOARD_TARGET))

UBOOT_DEFCONFIG ?= rock-pi-4-rk3399_defconfig
BL31 ?= tmp/atf/rk3399/22d12c4148c373932a7a81e5d1c59a767e143ac2.elf
BOARD_CHIP ?= rk3399
LOADER_BIN ?= tmp/rkbin/rk33/rk3399_loader_v1.10.112_support_1CS.bin
SPL_FLASH_OFFSET ?= 64
UBOOT_FLASH_OFFSET ?= 512
IMAGES ?= flash-spi erase-spi
FLASH_LOADER ?= rksd_loader
LOADERS ?= rksd_loader rkspi_loader

else ifeq (pinebookpro,$(BOARD_TARGET))

UBOOT_DEFCONFIG ?= pinebook_pro-rk3399_defconfig
BL31 ?= tmp/atf/rk3399/22d12c4148c373932a7a81e5d1c59a767e143ac2.elf
BOARD_CHIP ?= rk3399
LOADER_BIN ?= tmp/rkbin/rk33/rk3399_loader_v1.10.112_support_1CS.bin
SPL_FLASH_OFFSET ?= 64
UBOOT_FLASH_OFFSET ?= 512
IMAGES ?= flash-spi erase-spi
FLASH_LOADER ?= rksd_loader
LOADERS ?= rksd_loader rkspi_loader

else
$(error Unsupported BOARD_TARGET)
endif
