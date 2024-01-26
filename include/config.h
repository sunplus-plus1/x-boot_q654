#include <auto_config.h>

/**********************
 * Development Options
 *********************/

#if 0 // configure via "make config"
//////////////////////////////////////////////////
// Configurations for different iboot build
// * ASIC iBoot:  default
// * CSIM iBoot:  CSIM_NEW
// * Zebu iBoot:  CSIM_NEW + CONFIG_BOOT_ON_ZEBU
// * NOR_iBoot:   PLATFORM_SPIBAREMETAL
//////////////////////////////////////////////////
#endif

/* SPI Baremetal boot */
#ifdef CONFIG_XIP_SPIBAREMETAL
#define PLATFORM_SPIBAREMETAL	        /* Build for EXT_BOOT */
#endif

/* Emulation */
#define PLATFORM_SP7350                 /* Build for SP7350 */

/* CSIM build: Enable Stamp. No UART. Less delay. */
#ifdef CONFIG_BOOT_ON_CSIM
#define CSIM_NEW
#endif

/* Zebu build: speed up options */
#ifdef CONFIG_BOOT_ON_ZEBU
#define SPEED_UP_UART_BAUDRATE
#define SPEED_UP_SPI_NOR_CLK    /* speed up SPI_NOR flash (eg. Zebu) */
#endif

/* zmem support */
#ifdef CONFIG_USE_ZMEM
#define ZMEM_XBOOT_ADDR    0xFA200000
#endif

/***********************
 * xBoot
 ***********************/
#define XBOOT_BUILD           /* define in xboot build */

/**********************
 * Register
 *********************/
#define REG_BASE           0xF8000000
#define AHB0_REG_BASE      0xF8100000

#define RF_GRP(_grp, _reg)              ((((_grp) * 32 + (_reg)) * 4) + REG_BASE)
#define RF_AMBA(_grp, _reg)             ((((_grp) * 1024 + (_reg)) * 4) + REG_BASE)
#define AHB_GRP(_ahb_grp, _grp, _reg)   ((((_grp) * 32 + (_reg)) * 4) + ((_ahb_grp) * 0x1000) + AHB0_REG_BASE)

#define REG_BASE_AO       0xF8800000
#define RF_GRP_AO(_grp, _reg)           ((((_grp) * 32 + (_reg)) * 4) + REG_BASE_AO)
#define RF_AMBA_AO(_grp, _reg)          ((((_grp) * 1024 + (_reg)) * 4) + REG_BASE_AO)

#define A_REG_BASE                      0x9ec00000
#define A_RF_GRP(_grp, _reg)            ((((_grp) * 32 + (_reg)) * 4) + A_REG_BASE)

#define RF_MASK_V(_mask, _val)          (((_mask) << 16) | (_val))
#define RF_MASK_V_SET(_mask)            (((_mask) << 16) | (_mask))
#define RF_MASK_V_CLR(_mask)            (((_mask) << 16) | 0)

/**********************
 * Debug STAMP
 **********************/

#ifdef CSIM_NEW            // CSIM Stamp
#define STAMP(value)       REGS0(RF_GRP(0, 0), value);
#define CSTAMP(value)      { *(volatile unsigned int *)RF_GRP(0, 0) = (unsigned int)ADDRESS_CONVERT(value); }
#else                      // ASIC: No stamp
#define STAMP(value)       // empty
#define CSTAMP(value)      // empty
#endif

#define RID_PASS()         do { CSTAMP(0xabcddcba); } while (0)
#define RID_FAIL()         do { CSTAMP(0xdeaddaed); } while (0)
#define Z_DUMP_START()     do { CSTAMP(0xabcd1234); } while (0)

/**********************
 * Boot Mode
 *********************/

/* IV_MX[6:2] */
#define HW_CFG_REG              RF_GRP_AO(0, 31)

#define HW_CFG_SHIFT            12
#define HW_CFG_MASK_VAL         0x1F
#define HW_CFG_MASK             (HW_CFG_MASK_VAL << HW_CFG_SHIFT)

#define INT_BOOT                0x10
#define EMMC_BOOT               0x1F
#define SPINAND_BOOT            0x1D
#define USB_ISP                 0x1B
#define SDCARD_ISP              0x19
#define SPI_NOR_BOOT            0x17
#define UART_ISP                0x15
#define PARA_NAND_BOOT          0x11
#define USB_BOOT                0xfd
#define SDCARD_BOOT             0xfe
#define AUTO_SCAN               0xff

/************************************
 * Secure boot  xboot-->uboot
 ************************************/


/**********************
 * Clock
 *********************/
#define XTAL_CLK               (25 * 1000 * 1000)
#define PLLSYS                 (358 * 1000 * 1000)  /* 358 MHz is for CARD_CLK and CARD012_CLK */

/**********************
 * Timer
 *********************/
#define TIMER_KHZ           90

/**********************
 * ROM
 *********************/
#define BOOT_ROM_BASE       0xfffe8000
#define PROTECT_STA_ADDR    0xd000 /* ROM private section */
#define PROTECT_END_ADDR    0xffff /* ROM end */

/**********************
 * SPI
 *********************/
#define SPI_FLASH_BASE      0xF0000000
#define SPI_IBOOT_OFFSET    ( 0 * 1024)
#define SPI_XBOOT_OFFSET    (96 * 1024)

#define MAGIC_NUM_SPI_BAREMETAL 0x6D622B52

/**********************
 * SRAM
 *********************/
#define SRAM0_SIZE          (256 * 1024)

#define SRAM0_BASE          0xFA200000
#define SRAM0_END           (SRAM0_BASE + SRAM0_SIZE)


/* SRAM layout: must match with boot.ldi */

#define XBOOT_BUF_SIZE      (96 * 1024)
#define BOOTINFO_SIZE       (512)
#define GLOBAL_HEADER_SIZE  (512)
#define A64UP_SIZE          (1 * 1024)
#define CDATA_SIZE          (62 * 1024)
#define STORAGE_BUF_SIZE    (32 * 1024)
#define PAGE_TABLE_SIZE     (16 * 1024)
#define STACK_SIZE          (47 * 1024 - 64)
#define BOOTCOMPT_SIZE      (64)
#define RESERVED_RAM_SIZE   (1 * 1024)

#if defined(__aarch64__)
#define ISB()    do { asm volatile ("isb"); } while (0)
#define DSB()    do { asm volatile ("dsb sy"); } while (0)
#elif defined(__arm__)
#define ISB()    do { asm volatile ("isb"); } while (0)
#define DSB()    do { asm volatile ("dsb"); } while (0)
#endif

#define BOOT_RAM_BASE    SRAM0_BASE

#ifdef CONFIG_USE_ZMEM
#define XBOOT_ADDR       ZMEM_XBOOT_ADDR
#else
#define XBOOT_ADDR       BOOT_RAM_BASE
#endif

#define XBOOT_A64_ADDR   (BOOT_RAM_BASE + (97 * 1024))

/* for sp7350 warmboot */
#define WARMBOOT_XBOOT_ADDR     0x100000
#define WARMBOOT_XBOOT_SIZE     (4*1024)
#define WARMBOOT_A64_ADDR       (WARMBOOT_XBOOT_ADDR + WARMBOOT_XBOOT_SIZE)
#define WARMBOOT_A64_SIZE       (4*1024)


/**********************
 * CPU boot address
 *********************/

#define CPU_WAIT_A64_VAL             0xfffffffe
#define CORE_CPU_START_POS(core_id)  (CORE0_CPU_START_POS - ((core_id) * 4))
#define CORE3_CPU_START_POS          (0xfa23fc00 - 0x18)  // core3 wait fa23_fbe8
#define CORE2_CPU_START_POS          (0xfa23fc00 - 0x14)  // core2 wait fa23_fbec
#define CORE1_CPU_START_POS          (0xfa23fc00 - 0x10)  // core1 wait fa23_fbf0
#define CORE0_CPU_START_POS          (0xfa23fc00 - 0xc)   // core0 wait fa23_fbf4


#define CPU_WAIT_INIT_VAL        0xffffffff

#define B_START_POS              (SRAM0_END - 0x8)       // 9e809ff8
#define BOOT_ANOTHER_POS         (SRAM0_END - 0x4)       // 9e809ffc

#define A_START_POS_B_VIEW       (SRAM0_END - 0xc)
#define A_START_POS_A_VIEW       A_START_POS_B_VIEW
#define BOOT_ANOTHER_POS_A_VIEW  BOOT_ANOTHER_POS


/**********************
 * UART
 *********************/
#if defined(CONFIG_BOOT_ON_CSIM) || defined(CONFIG_BOOT_ON_ZEBU)
#define UART_SRC_CLK        (32 * 1000 * 1000 / 2)	// set 32M, for zebu
#else
#define UART_SRC_CLK        (XTAL_CLK)
#endif

/*
 * X = ((sclk + baud/2) / baud)
 * DIV_H = X  >> 12
 * DIV_L = (X & 0xf) << 12 | ((X >> 4) & 0xff - 1)
 */
#define UART_BAUD_DIV_H(baud, sclk)     ((((sclk) + ((baud) / 2)) / (baud)) >> 12)
#define UART_BAUD_DIV_L(baud, sclk)     ((((((sclk) + ((baud) / 2)) / (baud)) & 0xf) << 12) | \
					 ((((((sclk) + ((baud) / 2)) / (baud)) >> 4) & 0xff) - 1))
#ifdef SPEED_UP_UART_BAUDRATE
#define BAUDRATE            921600
#else
#define BAUDRATE            115200
#endif

#ifdef CSIM_NEW
#define UART_BT_TIMEOUT    (TIMER_KHZ)      /* 1ms */
#else
#define UART_BT_TIMEOUT    (TIMER_KHZ * 10) /* 10ms */
#endif

#define HAVE_PRINTF

#ifdef CONFIG_DEBUG_WITH_2ND_UART
#define DBG_UART_REG       UART1_REG
#else
#define DBG_UART_REG       UART0_REG
#endif

/***********************
 * NAND
***********************/

// Xboot skips nand init:
// Partial nand init only reads nand id
// (to makesure nand io ok after clock change)
#ifdef XBOOT_BUILD
#define PARTIAL_NAND_INIT
#endif

// If undefined, always use first uboot (uboot1)
// If defined, use 2nd uboot (uboot2) if ok ; fallback to uboot1 otherwise
#define HAVE_UBOOT2_IN_NAND

/***********************
* USB
***********************/
#define FAT_USB_4K_READ

#define USB3_PORT		0
#define USB2_PORT		1

/***********************
* SD CARD
***********************/
/* enable in iboot, disable in xboot */
//#define SD_VERBOSE

/* Card controller source clock */
#define CARD_CLK           (PLLSYS)
#define CARD012_CLK        (PLLSYS)

/***********************
* eMMC
***********************/
#define EMMC_SLOT_NUM   0
//#define EMMC_USE_DMA_READ	/* can't DMA to SRAM */

#define HAVE_UBOOT2_IN_EMMC	/* Use 2nd uboot if avaiable. Fallback to 1st uboot. */

/***********************
* OTP
***********************/
#define OTP_WHO_BOOT_REG	RF_GRP(4, 31)
#define OTP_WHO_BOOT_BIT	0

/***********************
* FIP partial
***********************/
#define NUM_OF_PARTITION        (111)
#define ISP_FILEINFO_OFFSET     (0x880)
#define EMMC_FIP_LBA            0x1022
#define NAND_FIP_OFFSET         (0x600000)

