/*
 * Copyright (C) 2011 ATLAS EMBEDDED SYSTEMS LTD.
 * Website : http://www.atlas-embedded.com
 * Release : r1_00
 * Date    : 2012-01-09
 * 
 * Based on da850evm.c.
 * 
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Based on da830evm.c. Original Copyrights follow:
 *
 * Copyright (C) 2009 Nick Thompson, GE Fanuc, Ltd. <nick.thompson@gefanuc.com>
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

#include <common.h>
#include <i2c.h>
#include <net.h>
#include <netdev.h>
#include <spi.h>
#include <spi_flash.h>
#include <asm/arch/nand_defs.h>
#include <asm/arch/hardware.h>
#include <asm/arch/emif_defs.h>
#include <asm/arch/emac_defs.h>
#include <asm/io.h>
#include <asm/errno.h>
#include "../common/misc.h"
#include "common.h"
#ifdef CONFIG_DAVINCI_MMC
#include <mmc.h>
#include <asm/arch/sdmmc_defs.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define pinmux(x)	(&davinci_syscfg_regs->pinmux[x])


/* SPI1 pinmux settings */
static const struct pinmux_config spi1_pins[] = {
	{ pinmux(5), 1, 1 }, // SPI1 CS0
	{ pinmux(5), 1, 2 }, // SPI1 CLK
	{ pinmux(5), 1, 4 }, // SPI1_SOMI
	{ pinmux(5), 1, 5 }  // SPI1_SIMO
};

#ifdef CONFIG_DAVINCI_MMC
/* MMC0 pinmux settings */
const struct pinmux_config mmc0_pins[] = {
	// { pinmux(0),  8, 4 },	/* GP0[11] is required for SD to work on Rev 3 EVMs */
	{ pinmux(10), 2, 0 },	// MMCSD0_CLK 
	{ pinmux(10), 2, 1 },	// MMCSD0_CMD 
	{ pinmux(10), 2, 2 },	// MMCSD0_DAT_0 
	{ pinmux(10), 2, 3 },	// MMCSD0_DAT_1 
	{ pinmux(10), 2, 4 },	// MMCSD0_DAT_2 
	{ pinmux(10), 2, 5 },	// MMCSD0_DAT_3
	/* DA850 supports only 4-bit mode, remaining pins are not configured */
};
#endif

/* UART2 pin mux settings */
static const struct pinmux_config uart_pins[] = {
	{ pinmux(0), 4, 6 }, // UART2_RTSn
	{ pinmux(0), 4, 7 }, // UART2_CTSn
	{ pinmux(4), 2, 4 }, // UART2_TXD
	{ pinmux(4), 2, 5 }  // UART2_RXD
};

// Bosphorus-I support only RMII PHY on board in default.
#ifdef CONFIG_DRIVER_TI_EMAC_USE_RMII
const struct pinmux_config emac_rmii_pins[] = {
	{ pinmux(14), 8, 2 },	// RMII_TXD1 
	{ pinmux(14), 8, 3 },	// RMII_TXD0 
	{ pinmux(14), 8, 4 },	// RMII_TXEN 
	{ pinmux(14), 8, 5 },	// RMII_RXD1 
	{ pinmux(14), 8, 6 },	// RMII_RXD0
	{ pinmux(14), 8, 7 },	// RMII_RXER 
	{ pinmux(15), 0, 0 },	// RMII_MHZ_50 (external 50.000MHz oscillator) 
	{ pinmux(15), 8, 1 },	// RMII_CRS_DV 
	{ pinmux(4), 8, 0 },	// MDIO_CLK 
	{ pinmux(4), 8, 1 }	// MDIO_D 
};
#endif

/* I2C0 pinmux settings */
static const struct pinmux_config i2c_pins[] = {
	{ pinmux(4), 2, 2 },  // I2C0_SCL
	{ pinmux(4), 2, 3 }   // I2C0_SDA
};

#ifdef CONFIG_NAND_DAVINCI
const struct pinmux_config nand_pins[] = {
	{ pinmux(7), 1, 1 },  // EMA_CS[3]n
	{ pinmux(7), 1, 2 },  // EMA_CS[4]n --> Only CS[3]n is used for Bosphorus-I (single die NAND)
	{ pinmux(7), 1, 4 },  // EMA_WEn
	{ pinmux(7), 1, 5 },  // EMA_OEn
	{ pinmux(9), 1, 0 },  // EMA_D[7]
	{ pinmux(9), 1, 1 },  // EMA_D[6]
	{ pinmux(9), 1, 2 },  // EMA_D[5]
	{ pinmux(9), 1, 3 },  // EMA_D[4]
	{ pinmux(9), 1, 4 },  // EMA_D[3]
	{ pinmux(9), 1, 5 },  // EMA_D[2]
	{ pinmux(9), 1, 6 },  // EMA_D[1]
	{ pinmux(9), 1, 7 },  // EMA_D[0]
	{ pinmux(12), 1, 5 }, // EMA_A[2] --> for CLE
	{ pinmux(12), 1, 6 }  // EMA_A[1] --> for ALE
};
// Bosphorus-I support NOR FLASH memory with MMCSD0 signals as upper address signals. 
#elif defined (CONFIG_SYS_USE_NOR)
const struct pinmux_config nor_pins[] = {	
	// { pinmux(0), 8, 4 }, // GP0[11] is required for SD to work on Rev 3 EVMs	
	{ pinmux(10), 8, 0 }, // GP4[7] for EMA_A[23]
	{ pinmux(5), 1, 6 },  // EMA_BA[1]
	{ pinmux(6), 1, 6 },  // EMA_WAIT[1]
	{ pinmux(7), 1, 0 },  // EMA_CS[2]
	{ pinmux(7), 1, 4 },  // EMA_WE
	{ pinmux(7), 1, 5 },  // EMA_OE
	// EMIFA Data Bus
	{ pinmux(8), 1, 0 }, // EMA_D[15] 
	{ pinmux(8), 1, 1 }, // EMA_D[14] 
	{ pinmux(8), 1, 2 }, // EMA_D[13] 
	{ pinmux(8), 1, 3 }, // EMA_D[12] 
	{ pinmux(8), 1, 4 }, // EMA_D[11] 
	{ pinmux(8), 1, 5 }, // EMA_D[10] 
	{ pinmux(8), 1, 6 }, // EMA_D[9] 
	{ pinmux(8), 1, 7 }, // EMA_D[8] 
	{ pinmux(9), 1, 0 }, // EMA_D[7] 
	{ pinmux(9), 1, 1 }, // EMA_D[6] 
	{ pinmux(9), 1, 2 }, // EMA_D[5] 
	{ pinmux(9), 1, 3 }, // EMA_D[4] 
	{ pinmux(9), 1, 4 }, // EMA_D[3] 
	{ pinmux(9), 1, 5 }, // EMA_D[2] 
	{ pinmux(9), 1, 6 }, // EMA_D[1] 
	{ pinmux(9), 1, 7 }, // EMA_D[0]
	// EMIFA Address Bus
	{ pinmux(10), 1, 0 }, // Reserved
	{ pinmux(10), 1, 1 }, // EMA_A[22]
	{ pinmux(10), 1, 2 }, // EMA_A[21]
	{ pinmux(10), 1, 3 }, // EMA_A[20]
	{ pinmux(10), 1, 4 }, // EMA_A[19]
	{ pinmux(10), 1, 5 }, // EMA_A[18]
	{ pinmux(10), 1, 6 }, // EMA_A[17]
	{ pinmux(10), 1, 7 }, // EMA_A[16]
	{ pinmux(11), 1, 0 }, // EMA_A[15]
	{ pinmux(11), 1, 1 }, // EMA_A[14]
	{ pinmux(11), 1, 2 }, // EMA_A[13]
	{ pinmux(11), 1, 3 }, // EMA_A[12]
	{ pinmux(11), 1, 4 }, // EMA_A[11]
	{ pinmux(11), 1, 5 }, // EMA_A[10]
	{ pinmux(11), 1, 6 }, // EMA_A[9]
	{ pinmux(11), 1, 7 }, // EMA_A[8]
	{ pinmux(12), 1, 0 }, // EMA_A[7]
	{ pinmux(12), 1, 1 }, // EMA_A[6]
	{ pinmux(12), 1, 2 }, // EMA_A[5]
	{ pinmux(12), 1, 3 }, // EMA_A[4]
	{ pinmux(12), 1, 4 }, // EMA_A[3]
	{ pinmux(12), 1, 5 }, // EMA_A[2]
	{ pinmux(12), 1, 6 }, // EMA_A[1]
	{ pinmux(12), 1, 7 }, // EMA_A[0]
};
#endif

// Bosphorus-I EMIFA default pinmux settings
// Using EMIFA pinmux settings does not affect NAND Flash usage, so that this structure can be used.
const struct pinmux_config emifa_pins[] = {
	// EMIFA Control Bus
	{ pinmux(6), 1, 7 }, // EMA_CS[0]
	{ pinmux(7), 1, 0 }, // EMA_CS[2]
	{ pinmux(7), 1, 1 }, // EMA_CS[3]
	{ pinmux(7), 1, 2 }, // EMA_CS[4]
	{ pinmux(7), 1, 3 }, // EMA_CS[5]
	{ pinmux(7), 1, 4 }, // EMA_WE
	{ pinmux(7), 1, 5 }, // EMA_OE
	{ pinmux(7), 1, 6 }, // EMA_RW
	{ pinmux(6), 1, 6 }, // EMA_WAIT[1]
	{ pinmux(5), 1, 7 }, // EMA_BA[0]CONFIG_DRIVER_TI_EMAC_USE_RMII
	{ pinmux(5), 1, 6 }, // EMA_BA[1]
	{ pinmux(6), 1, 5 }, // EMA_WE_DQM[1]
	{ pinmux(6), 1, 4 }, // EMA_WE_DQM[0]
	// EMIFA Data Bus
	{ pinmux(8), 1, 0 }, // EMA_D[15] 
	{ pinmux(8), 1, 1 }, // EMA_D[14] 
	{ pinmux(8), 1, 2 }, // EMA_D[13] 
	{ pinmux(8), 1, 3 }, // EMA_D[12] 
	{ pinmux(8), 1, 4 }, // EMA_D[11] 
	{ pinmux(8), 1, 5 }, // EMA_D[10] 
	{ pinmux(8), 1, 6 }, // EMA_D[9] 
	{ pinmux(8), 1, 7 }, // EMA_D[8] 
	{ pinmux(9), 1, 0 }, // EMA_D[7] 
	{ pinmux(9), 1, 1 }, // EMA_D[6] 
	{ pinmux(9), 1, 2 }, // EMA_D[5] 
	{ pinmux(9), 1, 3 }, // EMA_D[4] 
	{ pinmux(9), 1, 4 }, // EMA_D[3] 
	{ pinmux(9), 1, 5 }, // EMA_D[2] 
	{ pinmux(9), 1, 6 }, // EMA_D[1] 
	{ pinmux(9), 1, 7 }, // EMA_D[0]
	// EMIFA Address Bus
	{ pinmux(11), 1, 0 }, // EMA_A[15]
	{ pinmux(11), 1, 1 }, // EMA_A[14]
	{ pinmux(11), 1, 2 }, // EMA_A[13]
	{ pinmux(11), 1, 3 }, // EMA_A[12]
	{ pinmux(11), 1, 4 }, // EMA_A[11]
	{ pinmux(11), 1, 5 }, // EMA_A[10]
	{ pinmux(11), 1, 6 }, // EMA_A[9]
	{ pinmux(11), 1, 7 }, // EMA_A[8]
	{ pinmux(12), 1, 0 }, // EMA_A[7]
	{ pinmux(12), 1, 1 }, // EMA_A[6]
	{ pinmux(12), 1, 2 }, // EMA_A[5]
	{ pinmux(12), 1, 3 }, // EMA_A[4]
	{ pinmux(12), 1, 4 }, // EMA_A[3]
	{ pinmux(12), 1, 5 }, // EMA_A[2]
	{ pinmux(12), 1, 6 }, // EMA_A[1]
	{ pinmux(12), 1, 7 }, // EMA_A[0]
};

#ifdef CONFIG_DRIVER_TI_EMAC_USE_RMII
  #define HAS_RMII 1
#else
  #define HAS_RMII 0
#endif

static const struct pinmux_resource pinmuxes[] = {
#ifdef CONFIG_SPI_FLASH
	PINMUX_ITEM(spi1_pins),
#endif
	PINMUX_ITEM(uart_pins),
	PINMUX_ITEM(i2c_pins),
	PINMUX_ITEM(emac_rmii_pins), /* Added for Bosphorus-I Ethernet RMII pinmux support */
#ifdef CONFIG_NAND_DAVINCI
	PINMUX_ITEM(nand_pins),
	// PINMUX_ITEM(emifa_pins),  /* Added to configure to Bosphorus-I EMIFA pins, in this case */
								 /* NAND FLASH can be used without any pinmux settings. */
#elif defined(CONFIG_USE_NOR)
	PINMUX_ITEM(nor_pins),
#endif
};

static const struct lpsc_resource lpsc[] = {
	{ DAVINCI_LPSC_AEMIF },	/* NAND, NOR */
	{ DAVINCI_LPSC_SPI1 },	/* SPI NOR Flash */
	{ DAVINCI_LPSC_EMAC },	/* image download */
	{ DAVINCI_LPSC_UART2 },	/* console */
#ifdef VGA_SUPPORT	
	{ DAVINCI_LPSC_LCDC },  /* for VGA support */
#endif
	{ DAVINCI_LPSC_GPIO },
#ifdef CONFIG_DAVINCI_MMC
	{ DAVINCI_LPSC_MMC_SD },
#endif		
};



#ifndef CONFIG_BOSPHORUSI_MAX_CPU_CLK
//#define CONFIG_DA850_EVM_MAX_CPU_CLK	300000000
#define CONFIG_BOSPHORUSI_MAX_CPU_CLK	456000000
#endif

u32 get_board_type(void)
{
	if (i2c_probe(0x50) == 0)
		return 1;
	else
		return 0;
}

/*

 * get_board_rev() - setup to pass kernel board revision information
 * Returns:
 * bit[0-3]	Maximum cpu clock rate supported by onboard SoC
 *		0000b - 300 MHz
 *		0001b - 372 MHz
 *		0010b - 408 MHz
 *		0011b - 456 MHz
 */
u32 get_board_rev(void)
{
	char *s;
	u32 maxcpuclk = CONFIG_BOSPHORUSI_MAX_CPU_CLK;
	u32 rev = 0;
	u32 btype;

	s = getenv("maxcpuclk");
	if (s)
		maxcpuclk = simple_strtoul(s, NULL, 10);

	if (maxcpuclk >= 456000000)
		rev = 3;
	else if (maxcpuclk >= 408000000)
		rev = 2;
	else if (maxcpuclk >= 372000000)
		rev = 1;

	btype = get_board_type();

	if (btype == 1)
		rev |= 0x100;

	return rev;
}


/* BOARD INIT */
int board_init(void)
{
	
#ifndef CONFIG_USE_IRQ
	irq_init();
#endif


#ifdef CONFIG_NAND_DAVINCI
	/*
	 * NAND CS setup - cycle counts based on da850evm NAND timings 
	 */

	/* BosphorusI NAND settings  */
	writel((DAVINCI_ABCR_WSETUP(0) |
		DAVINCI_ABCR_WSTROBE(0) |
		DAVINCI_ABCR_WHOLD(0) |
		DAVINCI_ABCR_RSETUP(0) |
		DAVINCI_ABCR_RSTROBE(0) |
		DAVINCI_ABCR_RHOLD(0) |
		DAVINCI_ABCR_TA(2) |
		DAVINCI_ABCR_ASIZE_8BIT),
	       &davinci_emif_regs->ab2cr); 

	/* 
	davinci_emif_regs->ab2cr = (0 << 31)|(0 << 30)|(0x00 << 26)|(0x00 << 20)|(0x00  << 17)|
				   (0x00 << 13)|(0x00 << 7)|(0x00 << 4)| (2 << 2)| (0); 
	*/
#endif

	/* arch number of the board */
	gd->bd->bi_arch_number = MACH_TYPE_ATLAS_BOSPHORUSI;

	/* address of boot parameters */
	gd->bd->bi_boot_params = LINUX_BOOT_PARAM_ADDR;

	/*
	 * Power on required peripherals
	 * ARM does not have access by default to PSC0 and PSC1
	 * assuming here that the DSP bootloader has set the IOPU
	 * such that PSC access is available to ARM
	 */
	if (da8xx_configure_lpsc_items(lpsc, ARRAY_SIZE(lpsc)))
		return 1;

	/* setup the SUSPSRC for ARM to control emulation suspend */
	writel(readl(&davinci_syscfg_regs->suspsrc) &
	       ~(DAVINCI_SYSCFG_SUSPSRC_EMAC | DAVINCI_SYSCFG_SUSPSRC_I2C |
		 DAVINCI_SYSCFG_SUSPSRC_SPI1 | DAVINCI_SYSCFG_SUSPSRC_TIMER0 |
		 DAVINCI_SYSCFG_SUSPSRC_UART2),
	       &davinci_syscfg_regs->suspsrc);

	/* configure Bosphorus-I pinmux settings */
	if (davinci_configure_pin_mux_items(pinmuxes, ARRAY_SIZE(pinmuxes)))
		return 1;

#ifdef CONFIG_DAVINCI_MMC
	if (davinci_configure_pin_mux(mmc0_pins, ARRAY_SIZE(mmc0_pins)) != 0)
		return 1;
#endif

#ifdef CONFIG_DRIVER_TI_EMAC_USE_RMII
	if (davinci_configure_pin_mux(emac_rmii_pins, ARRAY_SIZE(emac_rmii_pins)) != 0)
		return 1;
	da850_emac_mii_mode_sel(HAS_RMII);
#endif

/* enable the console UART */
	writel((DAVINCI_UART_PWREMU_MGMT_FREE | DAVINCI_UART_PWREMU_MGMT_URRST |
		DAVINCI_UART_PWREMU_MGMT_UTRST),
	       &davinci_uart2_ctrl_regs->pwremu_mgmt);

	return 0;
}

#ifdef CONFIG_DRIVER_TI_EMAC
/*
 * Initializes on-board ethernet controllers.
 */
int board_eth_init(bd_t *bis)
{
	if (!davinci_emac_initialize()) {
		printf("Error: Ethernet init failed!\n");
		return -1;
	}

	return 0;
}
#endif 

/* CONFIG_DRIVER_TI_EMAC */

#define CFG_MAC_ADDR_SPI_BUS	0
#define CFG_MAC_ADDR_SPI_CS	0
#define CFG_MAC_ADDR_SPI_MAX_HZ	CONFIG_SF_DEFAULT_SPEED
#define CFG_MAC_ADDR_SPI_MODE	SPI_MODE_3
#define CFG_MAC_ADDR_OFFSET	(flash->size - SZ_64K)

static int  get_mac_addr(u8 *addr)
{
	int ret;
	struct spi_flash *flash;

	flash = spi_flash_probe(CFG_MAC_ADDR_SPI_BUS, CFG_MAC_ADDR_SPI_CS,
			CFG_MAC_ADDR_SPI_MAX_HZ, CFG_MAC_ADDR_SPI_MODE);
	if (!flash) {
		printf(" Error - unable to probe SPI flash.\n");
		goto err_probe;
	}

	ret = spi_flash_read(flash, CFG_MAC_ADDR_OFFSET, 6, addr);
	if (ret) {
		printf("Error - unable to read MAC address from SPI flash.\n");
		goto err_read;
	}

err_read:
	/* cannot call free currently since the free function calls free() for
	 * spi_flash structure though it is not directly allocated through
	 * malloc()
	 */
	/* spi_flash_free(flash); */
err_probe:
	return ret;
}

void dsp_lpsc_on(unsigned domain, unsigned int id)
{
	dv_reg_p mdstat, mdctl, ptstat, ptcmd;
	struct davinci_psc_regs *psc_regs;

	psc_regs = davinci_psc0_regs;
	mdstat = &psc_regs->psc0.mdstat[id];
	mdctl = &psc_regs->psc0.mdctl[id];
	ptstat = &psc_regs->ptstat;
	ptcmd = &psc_regs->ptcmd;

	while (*ptstat & (0x1 << domain))
		;

	if ((*mdstat & 0x1f) == 0x03)
		return;                 /* Already on and enabled */

	*mdctl |= 0x03;

	*ptcmd = 0x1 << domain;

	while (*ptstat & (0x1 << domain))
		;
	while ((*mdstat & 0x1f) != 0x03)
		;		/* Probably an overkill... */
}

static void dspwake(void)
{
	unsigned *resetvect = (unsigned *)DAVINCI_L3CBARAM_BASE;

	/* if the device is ARM only, return */
	if ((REG(CHIP_REV_ID_REG) & 0x3f) == 0x10)
		return;

	if (!strcmp(getenv("dspwake"), "no"))
		return;

	*resetvect++ = 0x1E000; /* DSP Idle */
	/* clear out the next 10 words as NOP */
	memset(resetvect, 0, sizeof(unsigned) * 10);

	/* setup the DSP reset vector */
	REG(HOST1CFG) = DAVINCI_L3CBARAM_BASE;

	dsp_lpsc_on(1, DAVINCI_LPSC_GEM);
	REG(PSC0_MDCTL + (15 * 4)) |= 0x100;
}

int misc_init_r(void)
{
	uint8_t tmp[20], addr[10];

	printf ("ARM Clock    : %d Hz\n", clk_get(DAVINCI_ARM_CLKID));
	printf ("DDR Clock    : %d Hz\n", clk_get(DAVINCI_DDR_CLKID)/2);
	printf ("EMIFA  Clock : %d Hz\n", clk_get(DAVINCI_EMIFA_CLKID));
	printf ("DSP    Clock : %d Hz\n", clk_get(DAVINCI_DSP_CLKID));
	printf ("ASYNC3 Clock : %d Hz\n", clk_get(ASYNC3));
	printf ("MMCSD0 Clock : %d Hz\n", clk_get(DAVINCI_MMCSD_CLKID));

	if (getenv("ethaddr") == NULL) {
		/* Read Ethernet MAC address from EEPROM */
		if (dvevm_read_mac_address(addr)) {
			/* Set Ethernet MAC address from EEPROM */
			davinci_sync_env_enetaddr(addr);
		} else {
			get_mac_addr(addr); // from SPI flash
		}

		if (is_multicast_ether_addr(addr) || is_zero_ether_addr(addr)) {
			printf("Invalid MAC address read.\n");
			return -EINVAL;
		}
		sprintf((char *)tmp, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0],
				addr[1], addr[2], addr[3], addr[4], addr[5]);

		setenv("ethaddr", (char *)tmp);
	}
	dspwake();
	return 0;
}

// Added to support NAND different options. (BBT, HW_ECC, OOBFIRST)
int board_nand_init(struct nand_chip *nand)
{
	davinci_nand_init(nand);

	return 0;
}

#ifdef CONFIG_DAVINCI_MMC
static struct davinci_mmc mmc_sd0 = {
	.reg_base = (struct davinci_mmc_regs *)DAVINCI_MMC_SD0_BASE,
	.host_caps = MMC_MODE_4BIT,     /* DA850 supports only 4-bit SD/MMC */
	.voltages = MMC_VDD_32_33 | MMC_VDD_33_34,
	.version = MMC_CTLR_VERSION_2,
};

int board_mmc_init(bd_t *bis)
{
	mmc_sd0.input_clk = clk_get(DAVINCI_MMCSD_CLKID);

	/* Add slot-0 to mmc subsystem */
	return davinci_mmc_init(bis, &mmc_sd0);
}
#endif
