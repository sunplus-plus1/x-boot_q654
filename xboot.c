#include <types.h>
#include <common.h>
#include <banner.h>
#include <bootmain.h>
#include <regmap.h>
#include <fat/fat.h>
#include <cpu/cpu.h>

#include <image.h>
#include <misc.h>
#include <otp/sp_otp.h>
#include <i2c/sp_i2c.h>
#include <sp_adc.h>
#include "fip.h"

#ifdef CONFIG_HAVE_EMMC
#include <sdmmc_boot/drv_sd_mmc.h>    /* initDriver_SD */
#include <part_efi.h>
#endif
#include <SECGRP_sp7350.h>

#include <hal_gpio.h>
#include <cpio.h>

#ifdef CONFIG_SECURE_BOOT_SIGN
#define SIGN_DATA_SIZE	(64)    // |header(64)|
#else
#define SIGN_DATA_SIZE (0)
#endif

#if defined(CONFIG_LPDDR4)
#define PLLD_800MHz
#elif defined(CONFIG_DDR4)
#define PLLD_666MHz
#elif defined(CONFIG_DDR3)
#define PLLD_466MHz
#endif
//#define PLLD_800MHz
//#define PLLD_666MHz
//#define PLLD_600MHz
//#define PLLD_533MHz
//#define PLLD_466MHz
//#define PLLD_400MHz
//#define PLLD_366MHz
//#define PLLD_333MHz
//#define PLLD_300MHz
//#define PLLD_266MHz
//#define PLLD_233MHz
//#define PLLD_200MHz

//#define CLKGEN_EMMC_400M
//#define CLKGEN_EMMC_800M
//#define CLKGEN_EMMC_358M
//#define CLKGEN_EMMC_716M

/*
 * TOC
 * ---------------------
 *  spi_nor_boot
 *  spi_nand_boot
 *  para_nand_boot
 *  emmc_boot
 *  usb_isp
 *  sdcard_isp
 */
#ifdef CONFIG_HAVE_OTP
extern void mon_rw_otp(void);
#endif

extern void *__etext, *__data, *__edata;
extern void *__except_stack_top;

__attribute__ ((section("storage_buf_sect")))    union storage_buf   g_io_buf;
__attribute__ ((section("bootinfo_sect")))       struct bootinfo     g_bootinfo;
__attribute__ ((section("boothead_sect")))       u8                  g_boothead[GLOBAL_HEADER_SIZE];
__attribute__ ((section("xboot_header_sect")))   u8                  g_xboot_buf[32];

fat_info g_finfo;

static void halt(void)
{
	while (1) {
		cpu_wfi();
	}
}


// SP7350 pin setting
// G_MX0 ~ G_MX49	: 0 ~ 49
// AO_MX0 ~ AO_MX48	: 50 ~ 98
// IV_MX0 ~ IV_MX6	: 99 ~ 105
static void set_pad_driving_strength(u32 pin, u32 strength)
{
	int reg_off = pin / 32;
	int bit_mask = 1 << (pin % 32);

	strength = (strength > 15) ? 15 : strength;

	if (strength & 1)
		PAD_CTL_REG->driving_selector0[reg_off] |= bit_mask;
	else
		PAD_CTL_REG->driving_selector0[reg_off] &= ~bit_mask;

	if (strength & 2)
		PAD_CTL_REG->driving_selector1[reg_off] |= bit_mask;
	else
		PAD_CTL_REG->driving_selector1[reg_off] &= ~bit_mask;

	if (strength & 4)
		PAD_CTL_REG->driving_selector2[reg_off] |= bit_mask;
	else
		PAD_CTL_REG->driving_selector2[reg_off] &= ~bit_mask;

	if (strength & 8)
		PAD_CTL_REG->driving_selector3[reg_off] |= bit_mask;
	else
		PAD_CTL_REG->driving_selector3[reg_off] &= ~bit_mask;
}

static void init_hw(void)
{
	int i;

	// enable CA55_SYS_TIMER
	volatile u32 *r = (void *)0xf810a000;
	r[2] = 0xfffffff0; // set cntl
	r[3] = 0xf; // set cntu
	r[0] = 0x3; // en=1 & hdbg=1

	// Set SPI-NOR to non-secure mode (secure_enable=0).
	*(volatile u32 *)(0xf8000b18) = *(volatile u32 *)(0xf8000b18) & 0xfffeffff;

	/* clken[all]  = enable */
	for (i = 0; i < sizeof(MOON2_REG_AO->clken) / 4; i++)
		MOON2_REG_AO->clken[i] = RF_MASK_V_SET(0xffff);

	/* gclken[all] = no */
	for (i = 0; i < sizeof(MOON2_REG_AO->gclken) / 4; i++)
		MOON2_REG_AO->gclken[i] = RF_MASK_V_CLR(0xffff);

	/* reset[all] = clear */
	for (i = 0; i < sizeof(MOON0_REG_AO->reset) / 4; i++)
		MOON0_REG_AO->reset[i] = RF_MASK_V_CLR(0xffff);

	#if defined(CONFIG_MT53E2G32D4_C)
	// Remap DRAM (0xf0000000 ~ 0xffffffff) to (0x300000000 ~ 0x30fffffff).
	MOON5_REG->sft_cfg[0] = RF_MASK_V((1 << 1), (1 << 1)); //for 8GB
	prn_string("Remap DRAM for 8GB\n");
	#else
	// Remap DRAM (0xf0000000 ~ 0xffffffff) to (0x100000000 ~ 0x10fffffff).
	MOON5_REG->sft_cfg[0] = RF_MASK_V((1 << 0), (1 << 0)); //for 4GB
	prn_string("Remap DRAM for 4GB\n");
	#endif
#if defined(CONFIG_REGULATOR_RT5759)
	#if !defined(CONFIG_BOOT_ON_CSIM) && !defined(CONFIG_BOOT_ON_ZEBU)
	// Set CA55 power (VDD_CA55) to 0.8V.
	// RT5759 is connected to I2C7 and its I2C address is 0x62.
	#define RT5759_I2C_CH   7
	#define RT5759_I2C_ADDR 0x62

	u8 buf[2];

	// I2C7,X1 (GPIO): 86, 87
	// Set driving strength to 6 (min: 6.8mA, typ: 9.9mA).
	set_pad_driving_strength(86, 6);
	set_pad_driving_strength(87, 6);

	// Initialize I2C7.
	sp_i2c_en(RT5759_I2C_CH, I2C_PIN_MODE0);
	_delay_1ms(1);

	// Read ID of RT5759. ID of RT5759 should be 0x82.
	buf[0] = 0;
	sp_i2c_write(RT5759_I2C_CH, RT5759_I2C_ADDR, buf, 1, SP_I2C_SPEED_STD);
	sp_i2c_read(RT5759_I2C_CH, RT5759_I2C_ADDR, buf, 1, SP_I2C_SPEED_STD);
	prn_string("ID of RT5759 = "); prn_byte0(buf[0]); prn_string("\n");
	if (buf[0] == 0x82) {
		buf[0] = 0x02;                  // Set VID to 0x14.
		buf[1] = 0x14;                  //
		sp_i2c_write(RT5759_I2C_CH, RT5759_I2C_ADDR, buf, 2, SP_I2C_SPEED_STD);
		_delay_1ms(1);
	}
	#endif
#elif defined(CONFIG_REGULATOR_STI8070X)
	// Set CA55 power (VDD_CA55) to 0.8V.
	// STI8070C is connected to I2C7 and its I2C address is 0x60.
	#define STI8070X_I2C_CH   7
	#define STI8070X_I2C_ADDR 0x60

	u8 id1=0;
	u8 id2=0;
	u8 buf[2];

	// I2C7,X1 (GPIO): 86, 87
	// Set driving strength to 6 (min: 6.8mA, typ: 9.9mA).
	set_pad_driving_strength(86, 6);
	set_pad_driving_strength(87, 6);

	// Initialize I2C7.
	sp_i2c_en(STI8070X_I2C_CH, I2C_PIN_MODE0);
	_delay_1ms(1);

	/*
	 * Read IDs of STI8070X.
	 * ID1 of STI8070X should be 0x88.
	 * ID2 of STI8070X should be 0x01.
	 */

	sp_i2c_restart_one(STI8070X_I2C_CH, STI8070X_I2C_ADDR, 0x03, &id1, 1, SP_I2C_SPEED_STD);
	prn_string("ID1 of STI8070X = "); prn_byte0(id1); prn_string("\n");

	sp_i2c_restart_one(STI8070X_I2C_CH, STI8070X_I2C_ADDR, 0x04, &id2, 1, SP_I2C_SPEED_STD);
	prn_string("ID2 of STI8070X = "); prn_byte0(id2); prn_string("\n");

	if (id1 == 0x88 && id2 == 0x01) {
		buf[0] = 0x00;                  // Set VSEL0 to 0x87.
		buf[1] = 0x87;
		sp_i2c_write(STI8070X_I2C_CH, STI8070X_I2C_ADDR, buf, 2, SP_I2C_SPEED_STD);
		_delay_1ms(1);
	}
#endif

	#if (0)
	u32 adc_buf[4];

	sp_adc_read(0, &adc_buf[0]);
	sp_adc_read(1, &adc_buf[1]);
	sp_adc_read(2, &adc_buf[2]);
	sp_adc_read(3, &adc_buf[3]);

	diag_printf("adc0 %d\n",adc_buf[0]);
	diag_printf("adc1 %d\n",adc_buf[1]);
	diag_printf("adc2 %d\n",adc_buf[2]);
	diag_printf("adc3 %d\n",adc_buf[3]);
	#endif

	// MS control-bits
	// Set to 1 if 1.8V is used.
	//PAD_DVIO_REG->gmx_21_27 = 1;		// SPI-NOR, SPI-NAND (X2), 8-bit NAND
	//PAD_DVIO_REG->gmx_28_37 = 1;		// eMMC, SPI-NAND (X2), 8-bit NAND
	//PAD_DVIO_REG->ao_mx_0_9 = 1;
	//PAD_DVIO_REG->ao_mx_10_19 = 1;
	//PAD_DVIO_REG->ao_mx_20_29 = 1;

	MOON3_REG_AO->clkgen[4] = RF_MASK_V((0x0F << 6), (0x00 << 6));   // Set source clock of I2C and SPI to 100 MHz and 400MHz, respectively.
	MOON3_REG_AO->clkgen[5] = RF_MASK_V((0x07 << 6), (0x00 << 6));   // Set AO system clock to 200 MHz
	//diag_printf("clkgen[4] 0x%x clkgen[4] addr 0x%x\n",MOON3_REG_AO->clkgen[4], &MOON3_REG_AO->clkgen[4]);
	//diag_printf("clkgen[5] 0x%x clkgen[5] addr 0x%x\n",MOON3_REG_AO->clkgen[5], &MOON3_REG_AO->clkgen[5]);

	/* reset stc config because AO sysclk is change from 25M to 200M */
	STC_init();
	AV1_STC_init();

	/* Set PLLC to 1.5G */
	prn_string("Set PLLC to 1.5GHz\n");
	MOON3_REG_AO->rsvd[1] = RF_MASK_V_SET(0x0001);                    // Switch CPU clock to PLLS_CK200M.
	MOON3_REG_AO->pllc_cfg[0] = RF_MASK_V((0xff << 7), (0x38 << 7));  // FBKDIV = 0x38
	MOON3_REG_AO->pllc_cfg[0] = RF_MASK_V_CLR(0x0018);                // PSTDIV = 0
	_delay_1ms(1);
	MOON3_REG_AO->rsvd[1] = RF_MASK_V_CLR(0x0001);                    // Switch CPU clock back to PLLC.
	//prn_string("PLLC[0] = "); prn_dword(MOON3_REG_AO->pllc_cfg[0]);
	//prn_string("PLLC[1] = "); prn_dword(MOON3_REG_AO->pllc_cfg[1]);

	/* Set PLLL3 to 1.2G */
	prn_string("Set PLLL3 to 1.2GHz\n");
	MOON3_REG_AO->rsvd[0] = RF_MASK_V((0xf << 12), (0x8 << 12));      // Switch L3 clock to PLLS_CK200M.
	MOON3_REG_AO->plll3_cfg[0] = RF_MASK_V((0xff << 7), (0x20 << 7)); // FBKDIV = 0x20
	MOON3_REG_AO->plll3_cfg[0] = RF_MASK_V_CLR(0x0018);               // PSTDIV = 0
	_delay_1ms(1);
	MOON3_REG_AO->rsvd[0] = RF_MASK_V((0xf << 12), (0x0 << 12));      // Switch L3 clock back to PLLL3.
	//prn_string("PLLL3[0] = "); prn_dword(MOON3_REG_AO->plll3_cfg[0]);
	//prn_string("PLLL3[1] = "); prn_dword(MOON3_REG_AO->plll3_cfg[1]);

	MOON4_REG_AO->sft_cfg[1] = RF_MASK_V_SET(0x80);                   // U3PHY SSC on

#if 0 // For C3V-W MIPI-CSI RX2/3
	/* Switch the shared analog macro to MIPI RX mode for MIPI-CSI RX2/3 */
	MOON4_REG_AO->sft_cfg[23] = RF_MASK_V((1 << 3), (1 << 3));

	#if 1
	/* This workaround is for MIPI-CSI RX3 malfunction issue. Enabling the BIST
	 * internal clock of MIPI-CSI RX2 port releases the reset of MIPI-CSI RX3
	 * port.
	 */
	*(volatile u32 *)0xf800539c = 0x00020003;		// BIST_INCLK_EN(G167.7[17])
	#endif
#endif

	*(volatile u32 *)ARM_TSGEN_WR_BASE = 3;          // EN = 1 and HDBG = 1
	*(volatile u32 *)(ARM_TSGEN_WR_BASE + 0x08) = 0; // CNTCV[31:0]
	*(volatile u32 *)(ARM_TSGEN_WR_BASE + 0x0C) = 0; // CNTCV[63:32]

#if 1 // RGMII interface
	PAD_CTL2_REG->gmac_softpad_ctrl[0] = 0x00025600; // GMAC TXC softpad (G102.30) = 0x00025600, Set TXC to softpad mode, delay 1.7 nS
	PAD_CTL2_REG->gmac_softpad_ctrl[1] = 0x80000000; // GMAC RXC softpad (G102.31) = 0x80000000, Set RXC to GPIO mode
	MOON3_REG_AO->clkgen[0] = RF_MASK_V_CLR(0x1000); // GMAC_PHYSEL (G3.23[12]) = 0, Set GMAC to use RGMII interface.
#else // RMII interface
	PAD_CTL2_REG->gmac_softpad_ctrl[0] = 0x00046000; // GMAC TXC softpad (G102.30) = 0x00046000, Set TXC to softpad, delay -2 nS
	PAD_CTL2_REG->gmac_softpad_ctrl[1] = 0x00000000; // GMAC RXC softpad (G102.31) = 0x00046000, Set RXC to softpad, delay 0 nS
	MOON3_REG_AO->clkgen[0] = RF_MASK_V_SET(0x1000); // GMAC_PHYSEL (G3.23[12]) = 1, Set GMAC to use RMII interface.
#endif

	// Turn off power of NPU (NPU_PWR_EN, GPIO65).
	GPIO_MASTER_REG->gpio_master[65 / 16] = 0x10001 << (65 % 16);
	GPIO_OUT_REG->gpio_out[65 / 16]       = 0x10000 << (65 % 16);
	GPIO_OE_REG->gpio_oe[65 / 16]         = 0x10001 << (65 % 16);
	PAD_CTL_REG->gpio_first[65 / 32]     |=       1 << (65 % 32);

	// Turn on power of Video codec (VV_PWR_EN, GPIO66).
	GPIO_MASTER_REG->gpio_master[66 / 16] = 0x10001 << (66 % 16);
	GPIO_OUT_REG->gpio_out[66 / 16]       = 0x10001 << (66 % 16);
	GPIO_OE_REG->gpio_oe[66 / 16]         = 0x10001 << (66 % 16);
	PAD_CTL_REG->gpio_first[66 / 32]     |=       1 << (66 % 32);

	// Turn on power of Main (MAIN_PWR_EN, GPIO67).
	GPIO_MASTER_REG->gpio_master[67 / 16] = 0x10001 << (67 % 16);
	GPIO_OUT_REG->gpio_out[67 / 16]       = 0x10001 << (67 % 16);
	GPIO_OE_REG->gpio_oe[67 / 16]         = 0x10001 << (67 % 16);
	PAD_CTL_REG->gpio_first[67 / 32]     |=       1 << (67 % 32);

#ifdef PLLD_800MHz
	prn_string("PLLD: 800MHz, DATARATE:3200\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x1008);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BE);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_666MHz
	prn_string("PLLD: 666.6MHz, DATARATE:2666\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x0808);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BE);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_600MHz
	prn_string("PLLD: 600MHz, DATARATE:2400\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x0408);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BD);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_533MHz
	prn_string("PLLD: 533.3MHz, DATARATE:2133.2\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x800A);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BD);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_466MHz
	prn_string("PLLD: 466.6MHz, DATARATE:1866.4\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x9812);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BF);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_400MHz
	prn_string("PLLD: 400MHz, DATARATE:1600\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x100A);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BC);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_366MHz
	prn_string("PLLD: 366.6MHz, DATARATE:1466.4\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x0C0A);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BC);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_333MHz
	prn_string("PLLD: 333.3MHz, DATARATE:1333\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x080A);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BC);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_300MHz
	prn_string("PLLD: 300MHz, DATARATE:1200\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x8412);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BD);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_266MHz
	prn_string("PLLD: 266.6MHz, DATARATE:1066.4\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x2012);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BD);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_233MHz
	prn_string("PLLD: 233.3MHz, DATARATE:933\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x1812);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BC);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
#ifdef PLLD_200MHz
	prn_string("PLLD: 200MHz, DATARATE:800\n");
	MOON3_REG_AO->plld_cfg[0] = RF_MASK_V(0xFFFF, 0x1012);
	MOON3_REG_AO->plld_cfg[1] = RF_MASK_V(0xFFFF, 0xC0BC);
	MOON3_REG_AO->plld_cfg[2] = RF_MASK_V(0xFFFF, 0x0107);
#endif
	prn_dword(MOON3_REG_AO->plld_cfg[0]);
	prn_dword(MOON3_REG_AO->plld_cfg[1]);
	prn_dword(MOON3_REG_AO->plld_cfg[2]);

	dbg();
}

static int run_draminit(void)
{
	/* skip dram init on csim/zebu */
	int save_val;
	int (*dram_init)(unsigned int);
#ifdef CONFIG_STANDALONE_DRAMINIT
	dram_init = (void *)DRAMINIT_RUN_ADDR;
	prn_string("standalone draiminit\n");
	dram_init = (void *)DRAMINIT_RUN_ADDR;
#else
	extern int dram_init_main(unsigned int);
	dram_init = (void *)dram_init_main;
#endif

	prn_string("Run draiminit@"); prn_dword((u32)ADDRESS_CONVERT(dram_init));
	save_val = g_bootinfo.mp_flag;

	dram_init(g_bootinfo.gbootRom_boot_mode);
	g_bootinfo.mp_flag = save_val;	/* restore prints */
	prn_string("Done draiminit\n");

#ifdef CONFIG_USE_ZMEM
	/* don't corrupt zmem */
	return 0;
#endif

	// put a brieft dram test
	if (dram_test()) {
		mon_shell();
		return -1;
	}

#if(0)   // AP6256 GPIO reset pin
	GPIO_MASTER_REG->gpio_master[53 / 16] = 0x10001 << (53 % 16);
	GPIO_OUT_REG->gpio_out[53 / 16] = 0x10000 << (53 % 16);
	GPIO_OE_REG->gpio_oe[53 / 16] = 0x10001 << (53 % 16);
	PAD_CTL_REG->gpio_first[53 / 32] |=  1 << (53 % 32);
	prn_string("set pin53 " __DATE__ " " __TIME__ "\n");
	GPIO_OUT_REG->gpio_out[53 / 16] = 0x10001 << (53 % 16);
#endif

	GPIO_MASTER_REG->gpio_master[56 / 16] = 0x10001 << (56 % 16);
	GPIO_OUT_REG->gpio_out[56 / 16] = 0x10000 << (56 % 16);
	GPIO_OE_REG->gpio_oe[56 / 16] = 0x10001 << (56 % 16);
	PAD_CTL_REG->gpio_first[56 / 32] |=  1 << (56 % 32);
	prn_string("set pin53 " __DATE__ " " __TIME__ "\n");
	GPIO_OUT_REG->gpio_out[56 / 16] = 0x10001 << (56 % 16);

	return 0;
}

static inline void release_spi_ctrl(void)
{
	// SPIFL no reset
	MOON0_REG_AO->reset[5] = RF_MASK_V_CLR(1 << 1); /* SPIFL_RESET=0 */
}

__attribute__((unused))
static void uhdr_dump(struct image_header *hdr)
{
	prn_string("magic=");
	prn_dword(image_get_magic(hdr));
	prn_string("hcrc =");
	prn_dword(image_get_hcrc(hdr));
	prn_string("time =");
	prn_dword(image_get_time(hdr));
	prn_string("size =");
	prn_dword(image_get_size(hdr));
	prn_string("load =");
	prn_dword(image_get_load(hdr));
	prn_string("entry=");
	prn_dword(image_get_ep(hdr));
	prn_string("dcrc =");
	prn_dword(image_get_dcrc(hdr));
	prn_string("name =");
	prn_string(image_get_name(hdr));
	prn_string("\n");
}

#ifdef CONFIG_HAVE_SPI_NOR

// return image data size (exclude header)
#ifdef CONFIG_USE_ZMEM
__attribute__((unused))
#endif
static int nor_load_uhdr_image(const char *img_name, void *dst, void *src, int verify)
{
	struct image_header *hdr;
	int i, len, step;

	prn_string("load "); prn_string(img_name);
	prn_string("@"); prn_dword((u32)ADDRESS_CONVERT(dst));
	prn_string("\n");

	dbg();
	memcpy32(dst, src, sizeof(*hdr)/4); // 64/4

	dbg();
	hdr = (struct image_header *)dst;

	dbg();
	// magic check
	if (!image_check_magic(hdr)) {
		prn_string("bad magic\n");
		return -1;
	}
	// check name
	if (memcmp((const u8 *)image_get_name(hdr), (const u8 *)img_name, strlen(img_name)) != 0) {
		prn_string("bad name\n");
		return -1;
	}
	// header crc
	if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		return -1;
	}

	// load image data
	len = image_get_size(hdr) + SIGN_DATA_SIZE;
	prn_string("load data size="); prn_decimal(len); prn_string("\n");

	/* copy chunk size */
#ifdef CSIM_NEW
	step = 2048;
#else
	step = 256 * 1024;
#endif

	for (i = 0; i < len; i += step) {
		prn_string(".");
		memcpy32(dst + sizeof(*hdr) + i, src + sizeof(*hdr) + i,
				(len - i < step) ? (len - i + 3) / 4 : step / 4);
	}
	prn_string("\n");

	// verify image data
	if (verify && !image_check_dcrc(hdr)) {
		prn_string("corrupted\n");
		return -1;
	}

	return len;
}

#ifdef CONFIG_STANDALONE_DRAMINIT
static int nor_load_draminit(void)
{
	struct xboot_hdr *xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET);
	int len;

	if (xhdr->magic != XBOOT_HDR_MAGIC) {
		prn_string("no xboot hdr\n");
		return -1;
	}

	// locate to where xboot.img.orig ends
	len = sizeof(struct xboot_hdr)  + xhdr->length;

	return nor_load_uhdr_image("draminit", (void *)DRAMINIT_LOAD_ADDR,
			(void *)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + len), 1);
}
#endif

static int nor_draminit(void)
{
#ifdef CONFIG_STANDALONE_DRAMINIT
	if (nor_load_draminit() <= 0) {
		prn_string("No draminit\n");
		return -1;
	}
	cpu_invalidate_icache_all();

#endif
	return run_draminit();
}
#endif /* CONFIG_HAVE_SPI_NOR */


#ifdef CONFIG_USE_ZMEM
#ifdef LOAD_SPLIT_INITRAMFS
static void zmem_check_initramfs(void)
{
	struct image_header *hdr;

	prn_string("[zmem] chk initramfs\n");
	hdr = (struct image_header *)INITRAMFS_LOAD_ADDR;
	if (!image_check_magic(hdr)) {
		prn_string("[zmem] no uhdr magic: "); prn_dword(image_get_magic(hdr));
		mon_shell();
	} else if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		mon_shell();
	}
}
#endif

static void zmem_check_dtb(void)
{
	struct image_header *hdr;

	prn_string("[zmem] chk dtb\n");
	hdr = (struct image_header *)DTB_LOAD_ADDR;
	if (!image_check_magic(hdr)) {
		prn_string("[zmem] no uhdr magic: "); prn_dword(image_get_magic(hdr));
		mon_shell();
	} else if (!image_check_dcrc(hdr)) {
		prn_string("corrupted\n");
		mon_shell();
	}
}

static void zmem_check_linux(void)
{
	struct image_header *hdr;

	prn_string("[zmem] chk linux\n");
	hdr = (struct image_header *)LINUX_LOAD_ADDR;
	if (!image_check_magic(hdr)) {
		prn_string("[zmem] no uhdr magic: "); prn_dword(image_get_magic(hdr));
		mon_shell();
	} else if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		mon_shell();
	}
}

static void zmem_check_uboot(void)
{
	struct image_header *hdr;
	int len;

	prn_string("[zmem] check uboot\n");
	hdr = (struct image_header *)UBOOT_LOAD_ADDR;
	if (!image_check_magic(hdr)) {
		prn_string("[zmem] no uhdr magic: "); prn_dword(image_get_magic(hdr));
		mon_shell();
	} else {
		len = image_get_size(hdr);
		prn_string("[zmem] uboot len="); prn_dword(len);
	}
}
#endif

static void cm4_init()
{
	/* CM4 init, boot in rootfs by remoteproc */
	prn_string("M4 init: \n");
	MOON0_REG_AO->reset[7]    = RF_MASK_V_SET(1 << 15); // reset M4
	MOON4_REG_AO->sft_cfg[19] = RF_MASK_V_SET(1 << 1);  // enable M4 address (highest 16 bits) remapping
#if 0 // START M4 in zmem mode for develop
#ifdef CONFIG_USE_ZMEM
	volatile u32 *m4_mem = (void *)CM4_BOOT_ADDR;
	prn_dword0((u32)&m4_mem[0]);   prn_string(": "); prn_dword(m4_mem[0]);
	prn_dword0((u32)&m4_mem[1]);   prn_string(": "); prn_dword(m4_mem[1]);
	prn_dword0((u32)&m4_mem[256]); prn_string(": "); prn_dword(m4_mem[256]);

	prn_string("... START M4 ...\n");
#endif
#endif

}
static int load_tfa_optee(void)
{
	uuid_t optee = UUID_SECURE_PAYLOAD_BL32;
	uuid_t bl31 = UUID_EL3_RUNTIME_FIRMWARE_BL31;
	uuid_t uuid_null = { {0} };

	fip_toc_header_t *fip_hdr = (fip_toc_header_t *)(FIP_LOAD_ADDR + sizeof(struct image_header));
	if(fip_hdr->name != TOC_HEADER_NAME)
	{
		mon_shell();
		return -1;
	}
	fip_toc_entry_t *fip_entry = (fip_toc_entry_t  *)((u32)fip_hdr + sizeof(fip_toc_header_t));

	while(memcmp((u8 *)&(fip_entry->uuid),(u8 *)&uuid_null,sizeof(uuid_t)) != 0)
	{
		if(memcmp((u8 *)&(fip_entry->uuid),(u8 *)&optee,sizeof(uuid_t)) == 0)
		{
			memcpy((u8 *)OPTEE_RUN_ADDR,(u8 *)((u32)fip_hdr + (u32)fip_entry->offset_address),fip_entry->size);
		}
		if(memcmp((u8 *)&(fip_entry->uuid),(u8 *)&bl31,sizeof(uuid_t)) == 0 )
		{
			memcpy((u8 *)BL31_RUN_ADDR,(u8 *)((u32)fip_hdr + (u32)fip_entry->offset_address),fip_entry->size);
		}
		fip_entry += 1;
	};
	return 0;
}

//TODO: Tune SOC security
// RD mnatis: http://psweb.sunplus.com/mantis_PD2/view.php?id=9092
static void set_module_nonsecure(void)
{
	int i;

	// Set RGST: allow access from non-secure
	for (i = 0; i < 32; i++) {
		RGST_SECURE_REG->cfg[i] = 0; // non-secure
	}

	// Set AMBA: allow access from non-secure
	AMBA_SECURE_REG->cfg[5] &= ~(3<<29); // u3phy0,1 non-secure

	// Set cbdma sram to be all non-secure
	SET_CBDMA0_S01(0);		// [0,       0] secure
	SET_CBDMA0_S02(0xffffffff);	// [256K, 256K] secure

	// Master IP : overwrite as secure(0) or non_secure(1)
	// 16-bit mask
	// 8-bit overwrite enable
	// 8-bit secure(0)/non_secure(1)
	SECGRP1_MAIN_REG->MAIN_NIC_S01 = 0xFFFFFF00;
	SECGRP1_MAIN_REG->MAIN_NIC_S02 = 0xFFFFFF00;
	CSTAMP(0xCBDA0003);
	SECGRP1_PAI_REG->PAI_NIC_S03  = 0xFFFFFF00;
	CSTAMP(0xCBDA0004);
	SECGRP1_PAII_REG->PAII_NIC_S04 = 0xFFFFFF00;
	SECGRP1_PAII_REG->PAII_NIC_S05 = 0xFFFFFF00;

}

// set optee-os memory to trust zone by TZC
void set_memory_secure(void)
{
	#define TZC_REGION_ID				(1)

	*(volatile u32 *)(0xf8c40100 + 0x20 * TZC_REGION_ID) = OPTEE_RUN_ADDR;                  // BASE_LOW
	*(volatile u32 *)(0xf8c40104 + 0x20 * TZC_REGION_ID) = 0x00000000;                      // BASE_HIGH
	*(volatile u32 *)(0xf8c40108 + 0x20 * TZC_REGION_ID) = OPTEE_RUN_ADDR + (0x200000 - 1); // TOP_LOW
	*(volatile u32 *)(0xf8c4010c + 0x20 * TZC_REGION_ID) = 0x00000000;                      // TOP_HIGH
	*(volatile u32 *)(0xf8c40110 + 0x20 * TZC_REGION_ID) = 0xc000000f;                      // ATTR: secure access enable
	*(volatile u32 *)(0xf8c40114 + 0x20 * TZC_REGION_ID) = 0x00000000;                      // ID_ACCESS disable
}

/*
 * Switch from aarch32 to aarch64.
 */

static void go_a32_to_a64(u32 ap_addr)
{
	extern void *__a64rom, *__a64rom_end;
	void *beg = (void *)&__a64rom;
	void *end = (void *)&__a64rom_end;
	u32 start64_addr;

	prn_string("32->64\n");

	set_module_nonsecure();

	set_memory_secure();

	// ap_addr will be used by BL31
	*(volatile u32 *)CORE0_CPU_START_POS = ap_addr;

	memcpy((void *)XBOOT_A64_ADDR, beg, end - beg);
	start64_addr = (u32)XBOOT_A64_ADDR;

	// xboot -> a64up -> BL31
	prn_string("a64up@"); prn_dword(start64_addr);

	// set aa64 boot address for all SMP cores
	SECGRP1_MAIN_REG->MAIN_CA55_S01 = start64_addr;
	SECGRP1_MAIN_REG->MAIN_CA55_S02 = start64_addr;
	SECGRP1_MAIN_REG->MAIN_CA55_S03 = start64_addr;
	SECGRP1_MAIN_REG->MAIN_CA55_S04 = start64_addr;

	DSB();

	// core 0 switches to AA64
	asm volatile ("mcr p15, 0, %0, c12, c0, 2" : : "r"(0x03));	// RR=1 AA64=1

	ISB();

	while (1) {
		asm volatile ("wfi");
	}
}

//#define TZC_TEST
#ifdef TZC_TEST
#define REG(a)		(*(volatile u32 *)(a))
#define TZC_BASE	0xf8c40000
#define TZC(offset)	REG(TZC_BASE + (offset))

static void ca55_dram_basic(u32 base, u32 mask)
{
	u32 addr, rdata, wdata;
	int i;

	prn_dword0(base); prn_string(": RW test ");
	for (i = 0; i < 32; i = i + 4) {
		addr = base + i;
		wdata = ((i+3)<<24) + ((i+2)<<16) + ((i+1)<<8) + i;
		REG(addr) = wdata;
		rdata = REG(addr);
		if (rdata != (wdata & mask)) {
			prn_string("fail!\n");
			return;
		}
	}
	prn_string("OK!\n");
}

static void tzc_test(void)
{
	// Region 1: 0x78000000 ~ 0x7fffffff
	TZC(0x120) = 0x78000000; // BASE_LOW
	TZC(0x124) = 0x00000000; // BASE_HIGH
	TZC(0x128) = 0x7fffffff; // TOP_LOW
	TZC(0x12c) = 0x00000000; // TOP_HIGH
	TZC(0x134) = 0x00000000; // ID_ACCESS disable

	TZC(0x130) = 0x0000000f; // ATTR: secure access disable
	delay_1ms(1);
	prn_string("Secure Disable: ");
	ca55_dram_basic(TZC(0x120), 0xffffffff);

	TZC(0x130) = 0xc000000f; // ATTR: secure access enable
	delay_1ms(1);
	prn_string("Secure Enable : ");
	ca55_dram_basic(TZC(0x120), 0xffffffff);
}
#endif

/* Assume u-boot has been loaded */
static void boot_uboot(void)
{
	__attribute__((unused)) int is_for_A = 0;
	const struct image_header *hdr = (struct image_header *)UBOOT_LOAD_ADDR;

#ifdef TZC_TEST
	tzc_test();
#endif

#ifdef CONFIG_SECURE_BOOT_SIGN
	prn_string("start verify in xboot!\n");
	int ret = xboot_verify_next_image((struct image_header *)UBOOT_LOAD_ADDR);
	if (ret) {
		mon_shell();
		halt();
	}
	ret = xboot_verify_next_image((struct image_header *)FIP_LOAD_ADDR);
	if (ret) {
		mon_shell();
		halt();
	}
#endif
	prn_string((const char *)image_get_name(hdr)); prn_string("\n");
	prn_string("Run uboot@");prn_dword(UBOOT_RUN_ADDR);
	/* boot aarch64 uboot */
	load_tfa_optee();
	cm4_init();
	go_a32_to_a64(UBOOT_RUN_ADDR);
}

#ifdef CONFIG_HAVE_SPI_NOR
#ifdef CONFIG_LOAD_LINUX

/* Assume dtb and uImage has been loaded */
static void boot_linux(void)
{
	CSTAMP(0x44556677);
	prn_string("Run Linux@");
	prn_dword(LINUX_RUN_ADDR);
	/* boot aarch64 kernel */
	go_a32_to_a64(LINUX_RUN_ADDR);
}

static void spi_nor_linux(void)
{
#ifdef CONFIG_USE_ZMEM
#ifdef LOAD_SPLIT_INITRAMFS
	zmem_check_initramfs();
#endif
	zmem_check_dtb();
	zmem_check_linux();
#else
	int res;
	int verify = 1;

#ifdef LOAD_SPLIT_INITRAMFS
#ifdef CONFIG_BOOT_ON_CSIM
	verify = 0; /* big */
#endif
	res = nor_load_uhdr_image("initramfs", (void *)INITRAMFS_LOAD_ADDR,
			(void *)(SPI_FLASH_BASE + SPI_INITRAMFS_OFFSET), verify);
	if (res <= 0) {
		prn_string("No initramfs!!!!!!!!!!!!!!!!!!!!!!!\n");
		//return;
	}
#endif

	verify = 1;
	res = nor_load_uhdr_image("dtb", (void *)DTB_LOAD_ADDR,
				  (void *)(SPI_FLASH_BASE + SPI_DTB_OFFSET), verify);
	if (res <= 0) {
		prn_string("No dtb\n");
		return;
	}

#ifdef CONFIG_BOOT_ON_CSIM
	verify = 0; /* big image */
#endif
	res = nor_load_uhdr_image("linux", (void *)LINUX_LOAD_ADDR,
				  (void *)(SPI_FLASH_BASE + SPI_LINUX_OFFSET), verify);
	if (res <= 0) {
		prn_string("No linux\n");
		return;
	}
#endif

	boot_linux();
}
#endif

static void spi_nor_uboot(void)
{
#ifdef CONFIG_USE_ZMEM
	zmem_check_uboot();
#else
	int len;
	len = nor_load_uhdr_image("fip", (void *)FIP_LOAD_ADDR,
				      (void *)(SPI_FLASH_BASE + SPI_FIP_OFFSET), 1);
	if (len <= 0 ){
		prn_string("load tfa optee failed \n");
		mon_shell();
		return;
	}
	len = nor_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR,
				      (void *)(SPI_FLASH_BASE + SPI_UBOOT_OFFSET), 1);
	if (len <= 0) {
		mon_shell();
		return;
	}
#endif

	boot_uboot();
}

static void spi_nor_boot(int pin_x)
{
	int i;

	// SPI-NOR (DVIO): 21, 22, 23, 24, 25, 26
	// Set driving strength of following pins to 4 (min: 5.6mA, typ: 15.2mA).
	for (i = 21; i <= 26; i++)
		set_pad_driving_strength(i, 4);

	// Set D0 (26) and D1 (23) to pull-down.
	PAD_CTL2_REG->dvio_pu[0] &= ~((1 << 6) | (1 << 3));	// bit 6 & 3 = 0
	PAD_CTL2_REG->dvio_pd[0] |= (1 << 6) | (1 << 3);	// bit 6 & 3 = 1
	delay_1ms(1);

#ifdef SPEED_UP_SPI_NOR_CLK
	dbg();
	SPI_CTRL_REG->spi_ctrl = (SPI_CTRL_REG->spi_ctrl & ~(7 << 16)) | (4 << 16); // 1: CLK_SPI/8

	SPI_CTRL_REG->spi_cfg[2] = 0x00150095; // restore default after setting spi_ctrl
#endif

	if (nor_draminit()) {
		dbg();
		return;
	}
	// spi linux
#ifdef CONFIG_LOAD_LINUX
	spi_nor_linux();
#endif

	spi_nor_uboot();
}
#endif /* CONFIG_HAVE_SPI_NOR */


#ifdef CONFIG_HAVE_FS_FAT
/* return image data size (exclude header) */
static int fat_load_uhdr_image(fat_info *finfo, const char *img_name, void *dst,
	u32 img_offs, int max_img_sz,int type)
{
	struct image_header *hdr = dst;
	int len,ret;
	int fileindex = 0;
	u8 *buf = g_io_buf.usb.sect_buf;

	prn_string("fat load ");
	prn_string(img_name);
	prn_string("\n");

	if ((type == USB_ISP)
		|| (type == USB_BOOT)
		) {
		/* USB DMA needs 4KiB-aligned address. */
		if ((u32)ADDRESS_CONVERT(dst) & 0xfff) {
			prn_string("WARN: unaligned dst "); prn_dword((u32)ADDRESS_CONVERT(dst));
		}
	}

	/* ISPBOOOT.BIN file index is 0,uboot.img is 1 fip.img is 2*/
	if(type == SDCARD_BOOT)
	{
		fileindex = (memcmp(img_name,"fip",strlen("fip")) != 0) ? FAT_UBOOT_INDEX : FAT_FIP_INDEX;
	}
	else
	{
		fileindex = FAT_ISPBOOOT_INDEX;
	}
	/* read header first */
	len = 64;
	ret = fat_read_file(fileindex, finfo, buf, img_offs, len, dst);
	if (ret == FAIL) {
		prn_string("load hdr failed\n");
		return -1;
	}

	/* magic check */
	if (!image_check_magic(hdr)) {
		prn_string("bad magic\n");
		return -1;
	}

	/* header crc */
	if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		return -1;
	}

	/* load image data */
	len = image_get_size(hdr);
	prn_string("load data size=");
	prn_decimal_ln(len);

	if (len + 64 > max_img_sz) {
		prn_string("image is too big, size=");
		prn_decimal(len + 64);
		return -1;
	}

	ret = fat_read_file(fileindex, finfo, buf, img_offs + 64, len + SIGN_DATA_SIZE, dst + 64);
	if (ret == FAIL) {
		prn_string("load body failed\n");
		return -1;
	}

	/* verify image data */
	if (!image_check_dcrc(hdr)) {
		prn_string("corrupted image\n");
		/* prn_crc(dst, len + 64, 4096); */
		return -1;
	}

	return len;
}
static int fat_load_fip(u32 type)
{
	int i=0,fip_offset=0;
	int ret;
	struct partition_info_s *isp_file_header = (struct partition_info_s *)(FIP_LOAD_ADDR + ISP_FILEINFO_OFFSET);
	u8 *buf = g_io_buf.usb.sect_buf;
	if(type != SDCARD_BOOT)
	{
		prn_string("load isp file header data \n");
		ret = fat_read_file(0, &g_finfo, buf, ISP_IMG_OFF_HEADER, 0x1000, (void *)FIP_LOAD_ADDR);
		if (ret == FAIL) {
			prn_string("load hdr failed\n");
			return -1;
		}
		prn_dword(*(volatile u32 *)FIP_LOAD_ADDR);
		if (*(volatile u32 *)FIP_LOAD_ADDR != 0x746E6550) {
			prn_string("bad magic\n");
			return -1;
		}
		while(isp_file_header[i].file_name[0] != 0)
		{
			if(memcmp(isp_file_header[i].file_name,"fip",strlen("fip")) == 0)
			{
				fip_offset = isp_file_header[i].file_offset;
				break;
			}
			i++;
		}
		if(i > NUM_OF_PARTITION)
		{
			prn_string("not found fip image! \n");
			return -1;
		}
	}
	if (fat_load_uhdr_image(&g_finfo, "fip", (void *)FIP_LOAD_ADDR, ((type==SDCARD_BOOT)?0:fip_offset), FIP_MAX_LEN,type) <= 0) {
		prn_string("failed to load fip \n");
		return -1;
	}
	return 0;
}

static void do_fat_boot(u32 type, u32 port)
{
	u32 ret;
#ifdef CONFIG_STANDALONE_DRAMINIT
	u8 *buf = (u8 *) g_io_buf.usb.draminit_tmp;
	struct xboot_hdr *xhdr = (struct xboot_hdr *)buf;
	int len;
#endif

	prn_string("finding file\n");

	ret = fat_boot(type, port, &g_finfo, g_io_buf.usb.sect_buf);
	if (ret == FAIL) {
		prn_string("no file\n");
		return;
	}

#ifdef CONFIG_STANDALONE_DRAMINIT
	/* nor_load_draminit(); */
	dbg();

	/* check xboot0 header to know draminit offset */
	len = 32;
	ret = fat_read_file(0, &g_finfo, g_io_buf.usb.sect_buf, ISP_IMG_OFF_XBOOT, len, buf);
	if (ret == FAIL) {
		prn_string("load xboot hdr failed\n");
		return;
	}

	if (xhdr->magic != XBOOT_HDR_MAGIC) {
		prn_string("xboot0 magic is wrong\n");
		return;
	}

	prn_string("xboot len=");
	prn_dword(32 + xhdr->length);

	/* draminit.img offset = size of xboot.img */
	len = fat_load_uhdr_image(&g_finfo, "draminit", buf, 32 + xhdr->length, sizeof(g_io_buf.usb.draminit_tmp));
	if (len <= 0) {
		prn_string("load draminit failed\n");
		return;
	}

	if ((u32)buf != (u32)DRAMINIT_LOAD_ADDR)
		memcpy32((u32 *) DRAMINIT_LOAD_ADDR, (u32 *) buf, (64 + len + 3) / 4);
#endif

	run_draminit();

	if (fat_sdcard_check_boot_mode(&g_finfo) == TRUE) {
		if (type == SDCARD_ISP) {
			g_bootinfo.gbootRom_boot_mode = SDCARD_BOOT;
			type = SDCARD_BOOT;
		} else if (type == USB_ISP) {
			g_bootinfo.gbootRom_boot_mode = USB_BOOT;
			type = USB_BOOT;
		}
	}

	if ((type == SDCARD_ISP) || (type == USB_ISP)) {
		int i;

		// SPI-NOR,    (DVIO): 21, 22, 23, 24, 25, 26
		// SPI-NAND,X1 (DVIO): 30, 31, 32, 33, 34, 35
		// SPI-NAND,X2 (DVIO): 21, 22, 23, 24, 25, 26
		// 8-bit NAND  (DVIO): 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36
		// eMMC        (DVIO): 20, 28, 29, 30, 31, 32, 33, 34, 35, 36
		// Set driving strength of following pins to 4 (min: 5.6mA, typ: 15.2mA).
		for (i = 20; i <= 36; i++)
			set_pad_driving_strength(i, 4);
		delay_1ms(1);

		/*
		 GPIO82=1 3.3V MS=0 default
		 GPIO82=0 1.8v MS=1
		*/
		HAL_GPIO_GPI(82);
		delay_1ms(1);
		if(HAL_GPIO_I_GET(82) == 0)
		{
			PAD_DVIO_REG->gmx_21_27 = 1;
			PAD_DVIO_REG->gmx_28_37 = 1;
			prn_string("gmx_21_27 gmx_28_37 = 1\n");
		}
		else
		{
			PAD_DVIO_REG->gmx_21_27 = 0;
			PAD_DVIO_REG->gmx_28_37 = 0;
			prn_string("gmx_21_27 gmx_28_37 = 0\n");
		}
		HAL_GPIO_RESET(82);
	}

#ifdef CONFIG_USE_ZMEM
	zmem_check_uboot();
#else
	/* load u-boot from usb */
	if(fat_load_fip(type) < 0)
	{
		prn_string("failed to load fip \n");
		return;
	}
	if (fat_load_uhdr_image(&g_finfo, "uboot", (void *)UBOOT_LOAD_ADDR, ((type==SDCARD_BOOT)?0:ISP_IMG_OFF_UBOOT), UBOOT_MAX_LEN,type) <= 0) {
		prn_string("failed to load uboot\n");
		return;
	}
#endif

	boot_uboot();
}
#endif /* CONFIG_HAVE_FS_FAT */

#if defined(CONFIG_HAVE_USB_DISK) || defined(CONFIG_HAVE_SNPS_USB3_DISK)
static void usb_isp(void)
{
	dbg();
	prn_string("\n{{usb}}\n");
	do_fat_boot(USB_ISP, g_bootinfo.bootdev_port);
}
#endif

#ifdef CONFIG_HAVE_SDCARD
static void sdcard_isp(void)
{
	prn_string("\n{{sdcard}}\n");
	do_fat_boot(SDCARD_ISP, 1);
}
#endif

#ifdef CONFIG_HAVE_UART_BOOTSTRAP
static void uart_isp(u32 forever)
{
	prn_string("\n{{uart_isp}}\n");
	mon_shell();
}
#endif

#ifdef CONFIG_HAVE_EMMC

#define EMMC_BLOCK_SZ 512     // bytes in a eMMC sector

static int emmc_read(u8 *buf, u32 blk_off, u32 count)
{
#ifdef EMMC_USE_DMA_READ
	/* dma mode supports multi-sector read */
	return ReadSDSector(blk_off, count, (unsigned int *)buf);
#else
	return ReadSDSector(blk_off, count, (unsigned int *)buf);
#endif
}

/**
 * emmc_load_uhdr_image
 * img_name       - image name in uhdr
 * dst            - destination address
 * loaded         - some data has been loaded
 * blk_off        - emmc block offset number
 * only_load_hdr  - only load header
 * size_limit     - image size in header > this limit, return error
 * mmc_part       - current active mmc part
 *
 * Return image data size (> 0) if ok (exclude header)
 */
static int emmc_load_uhdr_image(const char *img_name, u8 *dst, u32 loaded,
	u32 blk_off, int only_load_hdr, int size_limit, int mmc_part)
{
	struct image_header *hdr = (struct image_header *) dst;
	int len, res, blks;

	prn_string("emmc load "); prn_string(img_name);
	if (loaded) {
		prn_string("\nloaded=");
		prn_decimal_ln(loaded);
	}

	// load uhdr
	if (loaded < sizeof(*hdr)) {
		prn_string("@blk="); prn_dword(blk_off);
		res = emmc_read(dst + loaded, blk_off, 1);
		if (res) {
			prn_string("fail to read hdr\n");
			return -1;
		}
		loaded += EMMC_BLOCK_SZ;
		blk_off++;
	}

	// verify header
	if (!image_check_magic(hdr)) {
		prn_string("bad mg\n");
		return -1;
	}
	if (memcmp((const u8 *)image_get_name(hdr), (const u8 *)img_name, strlen(img_name)) != 0) {
		prn_string("bad name\n");
		return -1;
	}
	if (!image_check_hcrc(hdr)) {
		prn_string("bad hcrc\n");
		return -1;
	}

	len = image_get_size(hdr);

	if (only_load_hdr)
		return len;

	// load image data
	prn_string("data size="); prn_decimal_ln(len);
	if ((len <= 0) || (len + sizeof(*hdr)) >= size_limit) {
		prn_string("size > limit="); prn_decimal_ln(size_limit);
		return -1;
	}

	// load remaining
	res = sizeof(*hdr) + len + SIGN_DATA_SIZE - loaded;
	if (res > 0) {
		blks = (res + EMMC_BLOCK_SZ - 1) / EMMC_BLOCK_SZ;
		res = emmc_read(dst + loaded, blk_off, blks);
		if (res) {
			prn_string("failed to load data\n");
			return -1;
		}
	}

	// verify image data
	prn_string("verify img...\n");
	if (!image_check_dcrc(hdr)) {
		prn_string("corrupted\n");
		return -1;
	}

	return len;
}

#ifdef CONFIG_STANDALONE_DRAMINIT
int emmc_load_draminit(void *buf, int mmc_part)
{
	u32 sz_sect = EMMC_BLOCK_SZ;
	u32 xbsize, loaded;

	/* Because draminit.img is catenated to xboot.img,
	 * initial part of draminit.img may have been loaded by xboot's last sector */
	xbsize = get_xboot_size(g_xboot_buf);
	prn_string("xbsize="); prn_dword(xbsize);
	loaded = xbsize;
	while (loaded >= sz_sect) {
		loaded -= sz_sect;
	}
	if (loaded) {
		loaded = sz_sect - loaded;
	}
	prn_string("loaded="); prn_dword(loaded);
	if (loaded) {
		memcpy32((u32 *)buf, (u32 *)(g_xboot_buf + xbsize), loaded / 4);
	}

	/* Load remaining draminit.img from xboot's following sectors */

	if (emmc_load_uhdr_image("draminit", buf, loaded, g_bootinfo.app_blk_start, 0,
				 0x10000, mmc_part) <= 0) {
		dbg();
		return -1;
	}
	return 0;
}
#endif

static void emmc_boot(void)
{
	gpt_header *gpt_hdr;
	gpt_entry *gpt_part;
	u32 blk_start1 = -1, blk_start2 = -1;
	int res, len = 0;
	int i;

	prn_string("\n{{emmc_boot}}\n");
	SetBootDev(DEVICE_EMMC, 1, 0);

#ifdef CLKGEN_EMMC_400M
	MOON3_REG_AO->clkgen[0] = RF_MASK_V(3, 0); //CLKEMMC source is 400M
#endif
#ifdef CLKGEN_EMMC_800M
	MOON3_REG_AO->clkgen[0] = RF_MASK_V(3, 2); //CLKEMMC source is 800M
#endif
#ifdef CLKGEN_EMMC_358M
	MOON3_REG_AO->clkgen[0] = RF_MASK_V(3, 1); //CLKEMMC source is 358M
#endif
#ifdef CLKGEN_EMMC_716M
	MOON3_REG_AO->clkgen[0] = RF_MASK_V(3, 3); //CLKEMMC source is 716M
#endif
	// Set driving strength of following pins to 4 (min: 5.6mA, typ: 15.2mA).
	// eMMC (DVIO): 20, 28, 29, 30, 31, 32, 33, 34, 35, 36
	set_pad_driving_strength(20, 4);
	for (i = 28; i <= 36; i++)
		set_pad_driving_strength(i, 4);
	delay_1ms(1);

#ifdef CONFIG_STANDALONE_DRAMINIT
	/* continue to load draminit after iboot loading xboot */
	if (emmc_load_draminit(DRAMINIT_LOAD_ADDR, g_bootinfo.mmc_active_part)) {
		dbg();
		return;
	}
#endif

	if (run_draminit()) {
		return;
	}

#ifdef CONFIG_USE_ZMEM
	zmem_check_uboot();
#else
	if (initDriver_SD(EMMC_SLOT_NUM)) {
		prn_string("init fail\n");
		return;
	}

	/* mark active part */
	g_bootinfo.mmc_active_part = MMC_USER_AREA;
	if (SetMMCPartitionNum(0)) {
		prn_string("switch user area fail\n");
		return;
	}

	/* load uboot from GPT disk */
	prn_string("Read GPT\n");
	res = emmc_read(g_boothead, 1, 1); /* LBA 1 */
	if (res < 0) {
		prn_string("can't read LBA 1\n");
		return;
	}

	gpt_hdr = (gpt_header *)g_boothead;
	if (gpt_hdr->signature != GPT_HEADER_SIGNATURE) {
		prn_string("bad hdr sig: "); prn_dword((u32)gpt_hdr->signature);
		return;
	}

	res = emmc_read(g_boothead, 2, 1); /* LBA 2 */
	if (res < 0) {
		dbg();
		return;
	}
	gpt_part = (gpt_entry *)g_boothead;

	/* Look for uboot from GPT parts :
	 * part1 : xboot  / uboot1
	 * part2 : uboot1 / uboot2
	 * part3 : uboot2
	 * part4 : any
	 */
#ifndef HAVE_UBOOT2_IN_EMMC
	for (i = 0; i < 4; i++) {
		blk_start1 = (u32) gpt_part[i].starting_lba;
		prn_string("part"); prn_decimal(1 + i);
		prn_string(" LBA="); prn_dword(blk_start1);

		len = emmc_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR, 0,
					   blk_start1, 0, UBOOT_MAX_LEN, MMC_USER_AREA);
		if (len > 0)
			break;
	}
#else
	/* uboot1 - facotry default uboot image
	 * uboot2 - updated uboot image
	 * Logic:
	 * 1) Load uboot1 (header only)
	 * 2) Load uboot2
	 * 3) If uboot2 is not good, load uboot1
	 */
	for (i = 0; i < 4; i++) {
		if (blk_start1 == -1) {
			/* look for uboot1 */
			blk_start1 = (u32) gpt_part[i].starting_lba;
			prn_string("part"); prn_decimal(1 + i);
			prn_string(" LBA="); prn_dword(blk_start1);
			len = emmc_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR, 0,
						   blk_start1, 1, 0x200, MMC_USER_AREA);
			if (len > 0)
				prn_string("uboot1 hdr good\n");
			else
				blk_start1 = -1;
		} else {
			/* look for uboot2 */
			blk_start2 = (u32) gpt_part[i].starting_lba;
			prn_string("part"); prn_decimal(1 + i);
			prn_string(" LBA="); prn_dword(blk_start2);
			len = emmc_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR, 0,
						   blk_start2, 0, UBOOT_MAX_LEN, MMC_USER_AREA);
			if (len > 0) {
				prn_string("uboot2 good\n");
				break; /* good uboot2 */
			}
			blk_start2 = -1;
		}
	}
	/* fallback to uboot1 if no uboot2 */
	if ((blk_start2 == -1) && (blk_start1 != -1)) {
		prn_string("use uboot1\n");
		len = emmc_load_uhdr_image("uboot", (void *)UBOOT_LOAD_ADDR, 0,
					   blk_start1, 0, UBOOT_MAX_LEN, MMC_USER_AREA);
	}
#endif

	if (len <= 0) {
		prn_string("bad uboot\n");
		return;
	}
#endif

	len = emmc_load_uhdr_image("fip", (void *)FIP_LOAD_ADDR, 0,
				   EMMC_FIP_LBA, 0, FIP_MAX_LEN, MMC_USER_AREA);
	if (len <= 0) {
		prn_string("bad fip\n");
		return;
	}
	boot_uboot();
}
#endif /* CONFIG_HAVE_EMMC */

#ifdef CONFIG_HAVE_NAND_COMMON

/* bblk_read - Read boot block data
 * dst           - destination address
 * blk_off       - nand block offset
 * read_length   - length to read
 * max_blk_skip  - max bad blocks to skip
 * blk_last_read - last nand block read
 *
 * Return 0 if ok
 */
static int bblk_read(int type, u8 *dst, u32 blk_off, u32 read_length,
		     int max_blk_skip, u32 *blk_last_read)
{
	/* 'got' is used to record the size of the data has been read */
	u32 pg_off, length, got = 0;
	int i, j, blks, blk_skip = 0;
	int res;
	u32 sect_sz, blk_use_sz;

	/* prn_string("bblk_read blk="); prn_dword0(blk_off);
	 * prn_string(" len="); prn_decimal(read_length); prn_string("\n");
	 */

	sect_sz = GetNANDPageCount_1K60(g_bootinfo.sys_nand.u16PyldLen) * 1024;
	blk_use_sz = g_bootinfo.sys_nand.u16PageNoPerBlk * sect_sz;
	blks = (read_length + blk_use_sz - 1) / blk_use_sz;

	/* for each good block */
	for (i = 0; i < blks; i++) {
		if (blk_last_read) {
			*blk_last_read = blk_skip + blk_off + i;
		}

		pg_off = (blk_skip + blk_off + i) * g_bootinfo.sys_nand.u16PageNoPerBlk;

		/* for each page */
		for (j = 0; j < g_bootinfo.sys_nand.u16PageNoPerBlk; j++) {
#ifdef SKIP_BLOCK_WITH_BAD_BLOCK_MARK
			if (j == 0 || j == 1) {
				g_spareData[0] = 0xFF;
			}
#endif

			/* read sect */
#ifdef CONFIG_HAVE_SPI_NAND
			if (type == SPINAND_BOOT) {
				res = SPINANDReadNANDPage_1K60(0, pg_off + j, (u32 *)(dst + got), &length);
			}
#endif
#ifdef CONFIG_HAVE_PARA_NAND
			if (type == PARA_NAND_BOOT) {
				res = PNANDReadNANDPage_1K60(pg_off + j, (u32 *)(dst + got), &length);
			}
#endif

#ifdef SKIP_BLOCK_WITH_BAD_BLOCK_MARK
			/*
			 * Check bad block mark
			 *
			 * TODO: find bad block mark pos by id. Refer to nand_decode_bbm_options().
			 */
			if (j == 0 || j == 1) {
				if (g_spareData[0] != 0xFF) {
					prn_string("!! Skip bad block ");
					prn_dword(blk_skip + blk_off + i);
					if (j == 1) { /* Cancel page 0 data */
						got -= sect_sz;
					}

					/* prn_string("pg_off="); prn_decimal(pg_off); prn_string("\n");
					 * prn_dump_buffer(g_spareData, g_bootinfo.sys_nand.u16ReduntLen);
					 */

					if (++blk_skip > max_blk_skip) {
						prn_string("too many bad blocks!\n");
						return -1;
					}
					i--;
					break;
				}
			}
#endif

			/* good block but read page failed? */
			if (res) {
				dbg();
				prn_string("failed to read page #");
				prn_dword(pg_off + j);
				return -1;
			}

			got += length;
			if (got >= read_length) {
				/* dbg(); */
				return 0; /* ok */
			}
		}
	}

	dbg();
	return -1;
}

/* Search for image header */
static int bblk_find_image(int type, const char *name, u8 *dst, u32 blk_off,
			   u32 blk_cnt, u32 *found_blk)
{
	u32 i;
	int res;
	struct image_header *hdr = (struct image_header *)dst;

	/* Block (blk_off + i) has image? */
	for (i = 0; i < blk_cnt; i++) {
		/* prn_string("bblk_find_image blk="); prn_decimal(blk_off + i); prn_string("\n"); */

		res = bblk_read(type, dst, blk_off + i, 64, 50, NULL);
		if (res < 0)
			continue;

		/* magic */
		if (!image_check_magic(hdr)) {
			/* prn_string("bad magic\n"); */
			continue;
		}

		/* check name */
		if (memcmp((const u8 *)image_get_name(hdr), (const u8 *)name, strlen(name)) != 0) {
			prn_string("bad name\n");
			continue;
		}

		/* header crc */
		if (!image_check_hcrc(hdr)) {
			prn_string("bad hcrc\n");
			continue;
		}

		*found_blk = blk_off + i;
		prn_string("found hdr at blk=");
		prn_dword(*found_blk);
		return 0;
	}

	return -1;
}

/**
 * nand_load_uhdr_image
 * img_name       - image name in uhdr
 * dst            - destination address
 * blk_off        - nand block offset number
 * search_blk_cnt - max block count to search
 * img_blk_end    - returned the last nand block number of the image
 * only_load_hdr  - only load header
 *
 * Return image data size (> 0) if ok (exclude header)
 */
static int nand_load_uhdr_image(int type, const char *img_name, void *dst,
		u32 blk_off, u32 search_blk_cnt, u32 *img_blk_end, int only_load_hdr)
{
	struct image_header *hdr = (struct image_header *)dst;
	u32 len;
	u32 real_blk_off = 0;
	int res;

	prn_string("nand load "); prn_string(img_name);
	prn_string("@blk="); prn_dword(blk_off);

	/* find image header */
	res = bblk_find_image(type, img_name, (u8 *)hdr, blk_off, search_blk_cnt, &real_blk_off);
	if (res) {
		prn_string("image hdr not found\n");
		return -1;
	}

	/* uhdr_dump(hdr); */

	len = image_get_size(hdr);

	if (only_load_hdr)
		return len;

	/* load image data
	 * load data size = image header size + code image data size + signature data size
	 */
	prn_string("image size=");
	prn_decimal_ln(len);

	len += sizeof(struct image_header) + SIGN_DATA_SIZE;

	prn_string("load data size=");
	prn_decimal_ln(len);

	res = bblk_read(type, (u8 *)hdr, real_blk_off, len, 100, img_blk_end);
	if (res) {
		prn_string("failed to load data\n");
		return -1;
	}

	/* verify image data */
	prn_string("verify img...");
	if (!image_check_dcrc(hdr)) {
		prn_string("corrupted img\n");
		return -1;
	}
	prn_string("ok\n");

	return len; /* ok */
}

static void nand_uboot(u32 type)
{
#ifndef CONFIG_USE_ZMEM
	u32 blk_start = g_bootinfo.app_blk_start;
	u32 blk_end = 0;
	struct image_header *hdr = (struct image_header *)UBOOT_LOAD_ADDR;
	struct image_header hdr1;
	int len;
	u32 sect_sz = GetNANDPageCount_1K60(g_bootinfo.sys_nand.u16PyldLen) * 1024;
	u32 blk_use_sz = g_bootinfo.sys_nand.u16PageNoPerBlk * sect_sz;
#endif

	int i;

	if (type == SPINAND_BOOT) {
		// SPI-NAND,X1 (DVIO): 30, 31, 32, 33, 34, 35
		// SPI-NAND,X2 (DVIO): 21, 22, 23, 24, 25, 26
		// Set driving strength of following pins to 4 (min: 5.6mA, typ: 15.2mA).
		if (get_spi_nand_pinmux() == 2) {
			for (i = 21; i <= 26; i++)
				set_pad_driving_strength(i, 4);

			// Set D0 (26) and D1 (23) to pull-down.
			PAD_CTL2_REG->dvio_pu[0] &= ~((1 << 6) | (1 << 3));	// bit 6 & 3 = 0
			PAD_CTL2_REG->dvio_pd[0] |= (1 << 6) | (1 << 3);	// bit 6 & 3 = 1
		} else {
			for (i = 30; i <= 35; i++)
				set_pad_driving_strength(i, 4);

			// Set D0 (30) and D1 (33) to pull-down.
			PAD_CTL2_REG->dvio_pu[0] &= ~((1 << 13) | (1 << 10));	// bit 6 & 3 = 0
			PAD_CTL2_REG->dvio_pd[0] |= (1 << 13) | (1 << 10);	// bit 6 & 3 = 1
		}
	} else {
		// 8-bit NAND (DVIO): 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36
		// Set driving strength of following pins to 4 (min: 5.6mA, typ: 15.2mA).
		for (i = 21; i <= 36; i++)
			set_pad_driving_strength(i, 4);
	}
	delay_1ms(1);

#ifdef CONFIG_STANDALONE_DRAMINIT
	if (ReadBootBlockDraminit((type == SPINAND_BOOT), (u8 *)DRAMINIT_LOAD_ADDR) < 0) {
		prn_string("Failed to load nand draminit\n");
		return;
	}
#endif

	if (run_draminit()) {
		return;
	}

#ifdef CONFIG_USE_ZMEM
	zmem_check_uboot();
#else
#ifndef HAVE_UBOOT2_IN_NAND
	/* Load uboot1 from NAND */
	len = nand_load_uhdr_image(type, "uboot", (void *)UBOOT_LOAD_ADDR,
			blk_start, 10, &blk_end, 0);
#else
	/* uboot1 - facotry default uboot image
	 * uboot2 - updated uboot image
	 *
	 * Logic:
	 * 1) Load uboot1 header to guess uboot2 start block
	 * 2) Load uboot2
	 * 3) If uboot2 is not good, load uboot1
	 */

	memset((u8 *)&hdr1, 0, sizeof(hdr1));

	/* 1) uboot1 hdr */
	prn_string("Load uboot1 hdr\n");
	len = nand_load_uhdr_image(type, "uboot", (void *)hdr, blk_start, 10,
				&blk_end, 1);
	if (len <= 0) {
		prn_string("warn: not found uboot1\n");
		++blk_start; /* search for uboot2 since next block */
	} else {
		memcpy((u8 *)&hdr1, (const u8 *)hdr, sizeof(struct image_header));

		/* uboot2 follows uboot1
		 * uboot2 start blk = (uboot1 start blk) + (num of blocks of uboot1) */
		blk_start += (sizeof(struct image_header) + len + blk_use_sz - 1) / blk_use_sz;
	}
	/* 2) uboot2 (hdr + data) */
	prn_string("Load uboot2\n");
	len = nand_load_uhdr_image(type, "uboot", (void *)hdr, blk_start, 10, &blk_end, 0);
	if (len > 0) {
		prn_string("Use uboot2\n");
	} else if (image_get_size(&hdr1) > 0) {
		/* 3) uboot1 (fallback) hdr + data */
		prn_string("Fallback to uboot1\n");
		blk_start = g_bootinfo.app_blk_start;
		len = nand_load_uhdr_image(type, "uboot", (void *)hdr, blk_start,
				10, &blk_end, 0);
	}
#endif
	if (len <= 0) {
		prn_string("not found good uboot\n");
		return;
	}
#endif

	blk_start = (NAND_FIP_OFFSET + blk_use_sz - 1) / blk_use_sz / 2;

	hdr = (struct image_header *)FIP_LOAD_ADDR;
	len = nand_load_uhdr_image(type, "fip", (void *)hdr, blk_start,
			10, &blk_end, 0);
	if (len <= 0) {
		prn_string("not found fip\n");
		return;
	}
	boot_uboot();
}

#endif

#ifdef CONFIG_HAVE_PARA_NAND
static void para_nand_boot(int pin_x)
{
	u32 ret;
	prn_string("\n{{para_nand_boot}}\n");
	prn_decimal_ln(pin_x);

	SetBootDev(DEVICE_PARA_NAND, pin_x, 0);
	ret = InitDevice(PARA_NAND_BOOT);
	if (ret == ROM_SUCCESS) {
		nand_uboot(PARA_NAND_BOOT);
	}
	dbg();
}
#endif

#ifdef CONFIG_HAVE_SPI_NAND
static void spi_nand_boot(int pin_x)
{
	u32 ret;
	prn_string("\n{{spi_nand_boot}}\n");
	prn_decimal_ln(pin_x);

	SetBootDev(DEVICE_SPI_NAND, pin_x, 0);
	ret = InitDevice(SPINAND_BOOT);
	if (ret == ROM_SUCCESS) {
		nand_uboot(SPINAND_BOOT);
	}
	dbg();
}
#endif

void boot_not_support(void)
{
	diag_printf("Not support boot mode 0x%x in this build\n",
			g_bootinfo.gbootRom_boot_mode);
	mon_shell();
}

void custom_boot_flags( void) {
#ifdef CONFIG_CUSTOM_BOOT_BTN
	gpio_set_IO(CONFIG_CUSTOM_BOOT_BTN, 0, 0);
	if (!gpio_getV(CONFIG_CUSTOM_BOOT_BTN))
		return;
#if defined(CONFIG_HAVE_SDCARD) && (CONFIG_CUSTOM_BTN_DEV == SDCARD_ISP)
	SetBootDev(DEVICE_SDCARD, 1, 1);
#endif
#if defined(CONFIG_HAVE_USB_DISK) && (CONFIG_CUSTOM_BTN_DEV == USB_ISP)
	SetBootDev(DEVICE_USB_ISP, 0, 0);
#endif
	prn_string("\nCUSTOM KEY -> mode:");
	prn_dword(g_bootinfo.gbootRom_boot_mode);
#endif
}

static u32 read_hw_boot_mode(void)
{
	u32 mode;

	mode = *(volatile unsigned int *)HW_CFG_REG;
	return (mode & HW_CFG_MASK) >> HW_CFG_SHIFT;
}

/*
 * boot_flow - Top boot flow logic
 */
static void boot_flow(void)
{
	int retry = 10;
#ifdef OTP_TEST
	unsigned int i;
	unsigned int data = 0;
	char buf;
#endif

	/* Force romcode boot mode for xBoot testings :
	 * g_bootinfo.gbootRom_boot_mode = USB_ISP; g_bootinfo.bootdev = DEVICE_USB_ISP; g_bootinfo.bootdev_port = 1;
	 * g_bootinfo.gbootRom_boot_mode = EMMC_BOOT;
	 * g_bootinfo.gbootRom_boot_mode = UART_ISP;
	 * g_bootinfo.gbootRom_boot_mode = PARA_NAND_BOOT; g_bootinfo.app_blk_start = 2;
	 * g_bootinfo.gbootRom_boot_mode = SPI_NAND_BOOT;
	 * g_bootinfo.gbootRom_boot_mode = SDCARD_ISP; g_bootinfo.bootdev = DEVICE_SD0; g_bootinfo.bootdev_pinx = 1;
	 * prn_string("force boot mode="); prn_dword(g_bootinfo.gbootRom_boot_mode);
	 */

#ifdef OTP_TEST
	prn_string("OTP DATA\n");
	for (i = 0; i < 128; i++) {
		if (i < 64) {
			if (!otprx_read(HB_GP_REG, SP_OTPRX_REG, i, &buf)) {
				if ((i % 4) == 0) {
					prn_string("OTP"); prn_decimal(i / 4); prn_string(" = ");
					data = buf;
				} else if ((i % 4) == 1) {
					data = (((unsigned int) buf) << 8) | data;
				} else if ((i % 4) == 2) {
					data = (((unsigned int) buf) << 16) | data;
				} else if ((i % 4) == 3) {
					data = (((unsigned int) buf) << 24) | data;
					prn_dword(data);
				}
			}
		}
		else if ((i >= 64) && (i < 128)) {
			if (!otprx_key_read(OTP_KEY_REG, SP_OTPRX_REG, i, &buf)) {
				if ((i % 4) == 0) {
					prn_string("OTP"); prn_decimal(i / 4); prn_string(" = ");
					data = buf;
				} else if ((i % 4) == 1) {
					data = (((unsigned int) buf) << 8) | data;
				} else if ((i % 4) == 2) {
					data = (((unsigned int) buf) << 16) | data;
				} else if ((i % 4) == 3) {
					data = (((unsigned int) buf) << 24) | data;
					prn_dword(data);
				}
			}
		}
	}
#endif

	prn_string("mode=");
	prn_dword(g_bootinfo.gbootRom_boot_mode);

	// custom gpio flag handler
	custom_boot_flags();

	// NOR pins are enabled by hardware.
	// Release them if boot device is not NOR.
	if ((g_bootinfo.gbootRom_boot_mode != SPI_NOR_BOOT) &&
	    (read_hw_boot_mode() & INT_BOOT)){
		set_spi_nor_pinmux(0);
	}

	/* coverity[no_escape] */
	while (retry-- > 0) {
		/* Read boot mode */
		switch (g_bootinfo.gbootRom_boot_mode) {
		case UART_ISP:
#ifdef CONFIG_HAVE_UART_BOOTSTRAP
			uart_isp(1);
#endif
			break;
		case USB_ISP:
#if defined(CONFIG_HAVE_USB_DISK) || defined(CONFIG_HAVE_SNPS_USB3_DISK)
			usb_isp();
#endif
			break;
		case SDCARD_ISP:
#ifdef CONFIG_HAVE_SDCARD
			CSTAMP(0xC0DE000C);
			sdcard_isp();
#endif
			break;
		case SPI_NOR_BOOT:
#ifdef CONFIG_HAVE_SPI_NOR
			spi_nor_boot(g_bootinfo.bootdev_pinx);
#endif
			break;
		case SPINAND_BOOT:
#ifdef CONFIG_HAVE_SPI_NAND
			spi_nand_boot(g_bootinfo.bootdev_pinx);
#endif
			break;
		case PARA_NAND_BOOT:
#ifdef CONFIG_HAVE_PARA_NAND
			para_nand_boot(g_bootinfo.bootdev_pinx);
#endif
			break;
		case EMMC_BOOT:
#ifdef CONFIG_HAVE_EMMC
			emmc_boot();
#endif
			break;
		default:
			dbg();
			break;
		}
		boot_not_support();
	}
	prn_string("halt");
	halt();
}

static void init_uart(void)
{
#ifndef CONFIG_DEBUG_WITH_2ND_UART
	/* uart6 pinmux : UA6_TX, UA6_RX  used for CM4 debug message  */
	MOON1_REG_AO->sft_cfg[3] = RF_MASK_V((3 << 0), (1 << 0)); // UA6_SEL=1
	MOON0_REG_AO->reset[7] = RF_MASK_V_CLR(1 << 12);          // UA6_RESET=0
	UART6_REG->div_l = UART_BAUD_DIV_L(BAUDRATE, UART_SRC_CLK);
	UART6_REG->div_h = UART_BAUD_DIV_H(BAUDRATE, UART_SRC_CLK);
#if 0	// Enable UADBG for Linux ttyS5
	MOON1_REG_AO->sft_cfg[2] = RF_MASK_V_SET(1 << 14); /* UADBG_SEL=1 */
	MOON0_REG_AO->reset[5] = RF_MASK_V_CLR(1 << 10);   // UADBG_RESET=0
	UA2AXI_REG->axi_en = 0; // Disable UA2AXI and enable UADBG.
#endif
#endif
}

static inline void init_cdata(void)
{
	char *src = (char *)&__etext;
	char *dst = (char *)&__data;
	while (dst < (char *)&__edata) {
		*dst++ = *src++;
	}
}

static u32 read_mp_bit(void)
{
	char data = 0;
	u32  mp_bit;

#ifdef CONFIG_HAVE_OTP
	otprx_read(HB_GP_REG, SP_OTPRX_REG, 1, &data);
#endif
	mp_bit = (data >> 4) & 0x1;

	return mp_bit;
}

void check_ldo(void)
{
	char data = 0;

#ifdef CONFIG_HAVE_OTP
	otprx_read(HB_GP_REG, SP_OTPRX_REG, 68, &data);
#endif
	if(data & 0x01)
		*(volatile u32 *)(0xf88032d8) |= 0x00000004;		//SD LDO resistor mode
	data = 0;
#ifdef CONFIG_HAVE_OTP
	otprx_read(HB_GP_REG, SP_OTPRX_REG, 70, &data);
#endif
	if(data & 0x01)
		*(volatile u32 *)(0xf88032dc) |= 0x00000004;		//SDIO LDO resistor mod
}

void xboot_main(void)
{
	/* Initialize global data */
	init_cdata();

	g_bootinfo.in_xboot = 1;

	/* Is MP chip? Silent UART */
	g_bootinfo.mp_flag = read_mp_bit();

	init_uart();

	prn_decimal_ln(AV1_GetStc32());
	/* first msg */
	prn_string("+++xBoot " __DATE__ " " __TIME__ "\n");

	/* init hw */
	init_hw();

#ifdef MON
	mon_shell();
#endif

#ifdef CONFIG_HAVE_OTP
#ifdef CONFIG_HAVE_OTP_HS
	mon_rw_otp();
#endif
	check_ldo();
#endif

#if 0 // Enable CPIO master mode.
	cpio_master();
#endif
#if 0 // Enable CPIO slave mode.
	cpio_slave();
#endif
#ifdef CPIO_TEST
	cpio_test();
#endif

	/* start boot flow */
	boot_flow();

	/*
	 * build-time bug checker
	 */
	BUILD_BUG_ON(sizeof(union storage_buf) > STORAGE_BUF_SIZE);
}
