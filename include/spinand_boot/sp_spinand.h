/*
 * Sunplus Technology
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __SP_SPINAND_Q645_H
#define __SP_SPINAND_Q645_H

/*
 *  spi nand functional related configs
 */
#define CONFIG_SPINAND_TIMEOUT          100    /* unit: ms */
#define CONFIG_SPINAND_TRSMODE          SPINAND_TRS_DMA
#define CONFIG_SPINAND_READ_BITMODE     SPINAND_1BIT_MODE
#define CONFIG_SPINAND_CLK_DIV          4      /* 0~7 are allowed */
#if 0 // For zebu sim, set SPI read timing to 0.
#define CONFIG_SPINAND_READ_TIMING_SEL  0      /* 0~7 are allowed */
#else
#define CONFIG_SPINAND_READ_TIMING_SEL  2      /* 0~7 are allowed */
#endif

/*
 *  spi nand vendor ids
 */
#define VID_GD      0xC8
#define VID_WINBOND 0xEF
#define VID_TOSHIBA 0x98
#define VID_PHISON  0x6B
#define VID_ETRON   0xD5
#define VID_MXIC    0xC2
#define VID_ESMT    0xC8
#define VID_ISSI    0xC8
#define VID_MICRON  0x2C

/*
 *  SPINAND_CMD_*  are spi nand cmds.
 */
#define SPINAND_CMD_RESET            (0xff)
#define SPINAND_CMD_READID           (0x9f)
#define SPINAND_CMD_GETFEATURES      (0x0f)
#define SPINAND_CMD_SETFEATURES      (0x1f)
#define SPINAND_CMD_BLKERASE         (0xd8)
#define SPINAND_CMD_PAGE2CACHE       (0x13)
#define SPINAND_CMD_PAGEREAD         (0x03)
#define SPINAND_CMD_PAGEREAD_X2      (0x3b)
#define SPINAND_CMD_PAGEREAD_X4      (0x6b)
#define SPINAND_CMD_PAGEREAD_DUAL    (0xbb)
#define SPINAND_CMD_PAGEREAD_QUAD    (0xeb)
#define SPINAND_CMD_WRITEENABLE      (0x06)
#define SPINAND_CMD_WRITEDISABLE     (0x04)
#define SPINAND_CMD_PROGLOAD         (0x02)
#define SPINAND_CMD_PROGLOAD_X4      (0x32)
#define SPINAND_CMD_PROGEXEC         (0x10)

/*
 *  macros for spi_ctrl register
 */
#define SPINAND_BUSY_MASK            (1<<31)
#define SPINAND_AUTOMODE_EN          (1<<28)
#define SPINAND_AUTOCMD_EN           (1<<27)
#define SPINAND_SEL_CHIP_B           (1<<25)
#define SPINAND_SEL_CHIP_A           (1<<24)
#define SPINAND_AUTOWEL_EN           (1<<19)
#define SPINAND_SCK_DIV(x)           (((x)&0x07)<<16)
#define SPINAND_USR_CMD(x)           (((x)&0xff)<<8)
#define SPINAND_CTRL_EN              (1<<7)
#define SPINAND_USRCMD_DATASZ(x)     (((x)&0x07)<<4)   //0~3 are allowed
#define SPINAND_READ_MODE            (0<<2)            //read from device
#define SPINAND_WRITE_MODE           (1<<2)            //write to device
#define SPINAND_USRCMD_ADDRSZ(x)     (((x)&0x07)<<0)   //0~3 are allowed

/*
 *  macros for spi_timing register
 */
#define SPINAND_CS_SH_CYC(x)         (((x)&0x3f)<<22)
#define SPINAND_CD_DISACTIVE_CYC(x)  (((x)&0x3f)<<16)
#define SPINAND_READ_TIMING(x)       (((x)&0x07)<<1)  //0~7 are allowed
#define SPINAND_WRITE_TIMING(x)      (((x)&0x01))     //0~1 are allowed

/*
 *  macros for spi_status register
 */
#define SPINAND_STATUS_MASK          (0xff)

/*
 *  macros for spi_auto_cfg register
 */
#define SPINAND_USR_READCACHE_CMD(x) (((x)&0xff)<<24)
#define SPINAND_CONTINUE_MODE_EN     (1<<23)//enable winbon read continue mode
#define SPINAND_USR_CMD_TRIGGER      (1<<21)
#define SPINAND_USR_READCACHE_EN     (1<<20)
#define SPINAND_CHECK_OIP_EN         (1<<19)
#define SPINAND_AUTO_RDSR_EN         (1<<18)
#define SPINAND_DMA_OWNER_MASK       (1<<17)
#define SPINAND_DMA_TRIGGER          (1<<17)
#define SPINAND_DUMMY_OUT            (1<<16) //SPI_DQ is ouput in dummy cycles
#define SPINAND_USR_PRGMLOAD_CMD(x)  (((x)&0xff)<<8)
#define SPINAND_AUTOWEL_BF_PRGMEXEC  (1<<2)
#define SPINAND_AUTOWEL_BF_PRGMLOAD  (1<<1)
#define SPINAND_USR_PRGMLOAD_EN      (1<<0)

/*
 *  macros for spi_cfg0 register
 */
#define SPINAND_LITTLE_ENDIAN        (1<<23)
#define SPINAND_DATA64_EN            (1<<20)
#define SPINAND_TRS_MODE             (1<<19)
#define SPINAND_SCL_IDLE_LOW         (1<<17)
#define SPINAND_DATA_LEN(x)          (((x)&0xffff)<<0)

/*
 *  macros for spi_cfg1, spi_cfg2 register
 *  spi_cfg1 is used in pio(including auto mode) and dma mode.
 *  spi_cfg2 is used in memory access mode.
 */
#define SPINAND_DUMMY_CYCLES(x)      (((x)&0x3f)<<24)
/*
 *  bit mode selector:
 *     0 - no need tranfer
 *     1 - 1 bit mode
 *     2 - 2 bit mode
 *     3 - 4 bit mode
 */
#define SPINAND_DATA_BITMODE(x)      (((x)&0x03)<<20)
#define SPINAND_ADDR_BITMODE(x)      (((x)&0x03)<<18)
#define SPINAND_CMD_BITMODE(x)       (((x)&0x03)<<16)
/*
 *  cmd/addr/dataout DQ selector:
 *     0 - disabled
 *     1 - select DQ0
 *     2 - select DQ0,DQ1
 *     3 - select DQ0,DQ1,DQ2,DQ3
 */
#define SPINAND_DATAOUT_DQ(x)        (((x)&0x03)<<4)
#define SPINAND_ADDR_DQ(x)           (((x)&0x03)<<2)
#define SPINAND_CMD_DQ(x)            (((x)&0x03)<<0)
/*
 *  datain DQ selsector:
 *     0 - disabled
 *     1 - select DQ0
 *     2 - select DQ1
 */
#define SPINAND_DATAIN_DQ(x)         (((x)&0x03)<<6)

/*
 *  macros for spi_bch register
 */
#define SPINAND_BCH_DATA_LEN(x)      (((x)&0xff)<<8)
#define SPINAND_BCH_1K_MODE          (1<<6)
#define SPINAND_BCH_512B_MODE        (0<<6)
#define SPINAND_BCH_ALIGN_32B        (0<<5)
#define SPINAND_BCH_ALIGN_16B        (1<<5)
#define SPINAND_BCH_AUTO_EN          (1<<4)
#define SPINAND_BCH_BLOCKS(x)        (((x)&0x0f)<<0) //0~15 means 1~16 blocks

/*
 *  macros for spi_intr_msk,spi_intr_sts register
 */
#define SPINAND_PIO_DONE_MASK        (1<<2)
#define SPINAND_DMA_DONE_MASK        (1<<1)
#define SPINAND_BUF_FULL_MASK        (1<<0)

/*
 *  macros for spi_page_size register
 */
#define SPINAND_DEV_ECC_EN           (1<<15)
#define SPINAND_SPARE_SIZE(x)        (((x)&0x7ff)<<4)  //0~2047 are allowed.
#define SPINAND_PAGE_SIZE(x)         (((x)&0x07)<<0)   //0~7 means 1~8KB

enum SPINAND_TRSMODE {
	SPINAND_TRS_PIO,
	SPINAND_TRS_PIO_AUTO,
	SPINAND_TRS_DMA,
	SPINAND_TRS_DMA_AUTOBCH,
	SPINAND_TRS_MAX,
};

enum SPINAND_IO_MODE {
	SPINAND_1BIT_MODE,
	SPINAND_2BIT_MODE,
	SPINAND_4BIT_MODE,
	SPINAND_DUAL_MODE,
	SPINAND_QUAD_MODE,
};

/*block erase status */
#define ERASE_STATUS            0x04

/*protect status */
#define PROTECT_STATUS          0x38

/*
 *  spi nand device feature address
 */
#define DEVICE_PROTECTION_ADDR  0xA0
#define DEVICE_FEATURE_ADDR     0xB0
#define DEVICE_STATUS_ADDR      0xC0

/* Q645 spi nand driver */
struct sp_spinand_regs {
	unsigned int spi_ctrl;              // 87.0
	unsigned int spi_timing;            // 87.1
	unsigned int spi_page_addr;         // 87.2
	unsigned int spi_data;              // 87.3
	unsigned int spi_status;            // 87.4
	unsigned int spi_auto_cfg;          // 87.5
	unsigned int spi_cfg[3];            // 87.6
	unsigned int spi_data_64;           // 87.9
	unsigned int spi_buf_addr;          // 87.10
	unsigned int spi_statu_2;           // 87.11
	unsigned int spi_err_status;        // 87.12
	unsigned int mem_data_addr;         // 87.13
	unsigned int mem_parity_addr;       // 87.14
	unsigned int spi_col_addr;          // 87.15
	unsigned int spi_bch;               // 87.16
	unsigned int spi_intr_msk;          // 87.17
	unsigned int spi_intr_sts;          // 87.18
	unsigned int spi_page_size;         // 87.19
	unsigned int device_parity_addr;    // 87.20
};


struct sp_spinand_info {
	struct sp_spinand_regs *regs;
	u32 plane_sel_mode;
	u32 page_size;
	u32 oob_size;
	u32 spi_clk_div;
};

#endif /* __SP_SPINAND_Q645_H */

