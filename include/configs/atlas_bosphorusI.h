/*
 * Copyright (C) 2011 ATLAS EMBEDDED SYSTEMS LTD. - http://www.atlas-embedded.com
 * Website : http://www.atlas-embedded.com
 * Release : r01_00
 * Date    : 2012-01-09
 * 
 * based on DA850evm.h
 *  
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Based on davinci_dvevm.h. Original Copyrights follow:
 *
 * Copyright (C) 2007 Sergey Kubushyn <ksi@koi8.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * BosphorusI Board Types and selection
 */
#undef BOSPHORUSI_ONLY_SPIFLASH
#undef  BOSPHORUSI_ONLY_NAND
#undef  BOSPHORUSI_SPIFLASH_AND_NAND_1
#define  BOSPHORUSI_SPIFLASH_AND_NAND_2
// Type I : only SPI NOR FLASH
#ifdef BOSPHORUSI_ONLY_SPIFLASH
#define SPIFLASH_SUPPORT
#define CONFIG_USE_SPIFLASH
#endif
// Type II : only NAND FLASH
#ifdef BOSPHORUSI_ONLY_NAND
#define NANDFLASH_SUPPORT
#define CONFIG_USE_NAND
#endif
// Type III : SPI NOR FLASH with NAND FLASH, CONFIG in NAND FLASH
#ifdef BOSPHORUSI_SPIFLASH_AND_NAND_1
#define SPIFLASH_SUPPORT
#define NANDFLASH_SUPPORT
#undef CONFIG_USE_SPIFLASH
#define CONFIG_USE_NAND
#endif
// Type IV : SPI NOR FLASH with NAND FLASH, CONFIG in SPI NOR FLASH
#ifdef BOSPHORUSI_SPIFLASH_AND_NAND_2
#define SPIFLASH_SUPPORT
#define NANDFLASH_SUPPORT
#define CONFIG_USE_SPIFLASH
#undef CONFIG_USE_NAND
#endif

/*
 * Board
 */
#define CONFIG_DRIVER_TI_EMAC

/*
 * SoC Configuration
 */
#define CONFIG_MACH_ATLAS_BOSPHORUSI
#define CONFIG_ARM926EJS		/* arm926ejs CPU core */
#define CONFIG_SOC_DA8XX		/* TI DA8xx SoC */
#define CONFIG_SYS_CLK_FREQ		clk_get(DAVINCI_ARM_CLKID)
#define CONFIG_SYS_OSCIN_FREQ		24000000
#define CONFIG_SYS_TIMERBASE		DAVINCI_TIMER0_BASE
#define CONFIG_SYS_HZ_CLOCK		clk_get(DAVINCI_AUXCLK_CLKID)
#define CONFIG_SYS_HZ			1000
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_SYS_TEXT_BASE		0xc1080000

/*
 * Memory Info
 */
#define CONFIG_SYS_MALLOC_LEN		(0x10000 + 1*1024*1024) /* malloc() len 1MB */
#define PHYS_SDRAM_1			DAVINCI_DDR_EMIF_DATA_BASE /* DDR Start = 0xC000_0000 */
#define PHYS_SDRAM_1_SIZE		(128 << 20) /* DDR2 SDRAM, 128MB */
#define CONFIG_MAX_RAM_BANK_SIZE 	(512 << 20) /* max size, 512MB*/
#define CONFIG_SYS_MEMTEST_START	(PHYS_SDRAM_1 + 0x2000000) /* memtest start addr = Base + 32MB*/
#define CONFIG_SYS_MEMTEST_END 		(PHYS_SDRAM_1 + 0x2000000 + 64*1024*1024) /* memtest will be run on 64MB */
#define CONFIG_NR_DRAM_BANKS		1 /* we have 1 bank of DRAM */
#define CONFIG_STACKSIZE		(256*1024) /* regular stack, 256K */

/*
 * Serial Driver info
 */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	-4	/* NS16550 register size */
#define CONFIG_SYS_NS16550_COM1		DAVINCI_UART2_BASE /* Console, UART2 */
#define CONFIG_SYS_NS16550_CLK		clk_get(DAVINCI_UART2_CLKID)
#define CONFIG_CONS_INDEX		1		/* use for console */
#define CONFIG_BAUDRATE			115200		/* Default baud rate */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * SPI NOR Flash - IC support
 */
#ifdef SPIFLASH_SUPPORT
#define CONFIG_SPI
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_DAVINCI_SPI
#define CONFIG_SYS_SPI_BASE		DAVINCI_SPI1_BASE
#define CONFIG_SYS_SPI_CLK		clk_get(DAVINCI_SPI1_CLKID)
#define CONFIG_SF_DEFAULT_SPEED		30000000
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SF_DEFAULT_SPEED
#endif

/* 
SPI NOR Flash - CONFIG support
 */
#ifdef CONFIG_USE_SPIFLASH
#undef CONFIG_ENV_IS_IN_FLASH
#undef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SIZE			(64 << 10) /* UBOOT Env size 64KB */
#define CONFIG_ENV_OFFSET		(512 << 10) 
#define CONFIG_ENV_SECT_SIZE	(64 << 10) 
#define CONFIG_SYS_NO_FLASH
#endif

/*
 * NAND Flash - IC support
 */
#ifdef NANDFLASH_SUPPORT
#define CONFIG_NAND_DAVINCI
#define CONFIG_SYS_NO_FLASH
#define	CONFIG_SYS_NAND_USE_FLASH_BBT
#define CONFIG_SYS_NAND_4BIT_HW_ECC_OOBFIRST /* 8-bit NAND  support 4 bit ECC*/
#define	CONFIG_SYS_NAND_PAGE_2K				 // Page size of Bosphorus-I Micron NAND Flash
#define CONFIG_SYS_NAND_CS		3  			 // Bootable Flash
#define CONFIG_SYS_NAND_BASE		DAVINCI_ASYNC_EMIF_DATA_CE3_BASE
#define CONFIG_SYS_CLE_MASK		0x10
#define CONFIG_SYS_ALE_MASK		0x8
#undef CONFIG_SYS_NAND_HW_ECC          /* turn off 4-bit NAND HW ECC support */
#define CONFIG_SYS_MAX_NAND_DEVICE	1 /* Max number of NAND devices */
#define NAND_MAX_CHIPS			1
#define CONFIG_SYS_NAND_QUIET_TEST
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#undef CONFIG_MTD_DEBUG
#ifdef CONFIG_MTD_DEBUG
#define CONFIG_MTD_DEBUG_VERBOSE	4
#endif
#define CONFIG_JFFS2_NAND 1
#endif

/*
 * NAND Flash - CONFIG support
 */
#ifdef CONFIG_USE_NAND
#undef CONFIG_ENV_IS_IN_FLASH
#undef CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_NAND_DAVINCI
#define CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_IS_IN_NAND		/* U-Boot env in NAND Flash  */
#define CONFIG_ENV_OFFSET		0x0 /* Block 0--not used by bootcode */
#define CONFIG_ENV_SIZE			(128 << 10) /* UBOOT Env size 128KB */
#endif

/*
 * I2C Configuration
 */
#define CONFIG_HARD_I2C
#define CONFIG_DRIVER_DAVINCI_I2C
#define CONFIG_SYS_I2C_SPEED		25000
#define CONFIG_SYS_I2C_SLAVE		10 /* Bogus, master-only in U-Boot */
// #define CONFIG_SYS_I2C_EXPANDER_ADDR	0x20   /* No I2C IO Expander */

/*
 * VGA Support (for test)
 */
#undef VGA_SUPPORT
/*
 * Network & Ethernet Configuration
 */
#ifdef  CONFIG_DRIVER_TI_EMAC
#define CONFIG_EMAC_MDIO_PHY_NUM	0
#define CONFIG_MII
#define	CONFIG_DRIVER_TI_EMAC_USE_RMII		// Use RMII mode instead of MII mode 
#define CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DNS
#define CONFIG_BOOTP_DNS2
#define CONFIG_BOOTP_SEND_HOSTNAME
#define CONFIG_NET_RETRY_COUNT	10
#define CONFIG_NET_MULTI
#endif

/*
 * U-Boot general configuration
 */
// #undef CONFIG_USE_IRQ	/* No IRQ support in U-Boot */
#define CONFIG_MISC_INIT_R
#define CONFIG_BOOTFILE		"uImage" /* Boot file name */
#define CONFIG_SYS_PROMPT	"BosphorusI_U-Boot > " /* Command Prompt */
#define CONFIG_SYS_CBSIZE	1024 /* Console I/O Buffer Size	*/
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16 /* max number of command args */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE /* Boot Args Buffer Size */
#define CONFIG_SYS_LOAD_ADDR	(PHYS_SDRAM_1 + 0x700000) /* plus 7M */
#define CONFIG_VERSION_VARIABLE
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LONGHELP
#define CONFIG_CRC32_VERIFY
#define CONFIG_MX_CYCLIC
#define CONFIG_ENV_OVERWRITE 	/* allow to change the MAC address */

/*
 * Linux Information
 */
#define LINUX_BOOT_PARAM_ADDR	(PHYS_SDRAM_1 + 0x100)
#define CONFIG_CMDLINE_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_BOOTARGS		"mem=128M console=ttyS2,115200n8 root=/dev/ram0 rw initrd=0xc1180000,4m ip=off"
#define CONFIG_BOOTCOMMAND	"sf probe 0; sf read 0xc0700000 0xA0000 0x200000;bootm 0xc0700000" /* for SPI flash */
#define CONFIG_BOOTDELAY	3

/*
 * U-Boot commands
 */
#include <config_cmd_default.h>
#define CONFIG_CMD_ENV
#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DIAG
#define CONFIG_CMD_MII
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVES
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_I2C
#define CONFIG_CMD_SAVEENV
#define CONFIG_CMD_JFFS2

#ifndef CONFIG_DRIVER_TI_EMAC
#undef CONFIG_CMD_NET
#undef CONFIG_CMD_DHCP
#undef CONFIG_CMD_MII
#undef CONFIG_CMD_PING
#endif

#ifdef NANDFLASH_SUPPORT
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
#define CONFIG_CMD_NAND
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_LZO
#define CONFIG_RBTREE
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#endif

#ifdef CONFIG_USE_NAND
#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
#define CONFIG_CMD_NAND
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_LZO
#define CONFIG_RBTREE
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#endif

#ifdef SPIFLASH_SUPPORT
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_FLASH
#define CONFIG_CMD_SPI
#define CONFIG_CMD_SF
#define CONFIG_CMD_SAVEENV
#endif

#ifdef CONFIG_USE_SPIFLASH
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_FLASH
#define CONFIG_CMD_SPI
#define CONFIG_CMD_SF
#define CONFIG_CMD_SAVEENV
#endif



/* 
 * SD/MMC Configuration
 */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_DAVINCI_MMC
#define CONFIG_MMC_MBLOCK

#ifdef CONFIG_MMC
#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_CMD_MMC
#undef CONFIG_ENV_IS_IN_MMC
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
#undef CONFIG_ENV_SIZE
#undef CONFIG_ENV_OFFSET
#define CONFIG_ENV_SIZE		(16 << 10)	/* 16 KiB */
#define CONFIG_ENV_OFFSET	(51 << 9)	/* Sector 51 */
#undef CONFIG_ENV_IS_IN_FLASH
#undef CONFIG_ENV_IS_IN_NAND
#undef CONFIG_ENV_IS_IN_SPI_FLASH
#endif

/* 
 * USB Configuration
 */
#define CONFIG_USB_DA8XX
#define CONFIG_MUSB_HCD
#define CONFIG_USB_STORAGE	

#ifdef CONFIG_USB_DA8XX
#define CONFIG_USB_STORAGE	/* support for MSC class support */
#define CONFIG_CMD_USB		/* support for usb       */
#define CONFIG_CMD_STORAGE	/* support for usb       */
#define CONFIG_CMD_FAT		/* support for FAT/storage*/
#define CONFIG_DOS_PARTITION	/* support for FAT/storage*/
#endif



/* additions for new relocation code, must added to all boards */
#define CONFIG_SYS_SDRAM_BASE		0xc0000000
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_SDRAM_BASE + 0x1000 - /* Fix this ?? */ \
					GENERATED_GBL_DATA_SIZE)
#endif /* __CONFIG_H */
