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
// * 8388_iBoot:  PLATFORM_SPIBAREMETAL & PLATFORM_8388
//////////////////////////////////////////////////
#endif

/* SPI Baremetal boot */
#ifdef CONFIG_XIP_SPIBAREMETAL
#define PLATFORM_SPIBAREMETAL	        /* Build for EXT_BOOT */
#endif

/* Emulation */
#if defined(CONFIG_PLATFORM_Q628)
#define PLATFORM_Q628                   /* Build for Q628 */
#elif defined(CONFIG_PLATFORM_Q645)
#define PLATFORM_Q645                   /* Build for Q645 */
#elif defined(CONFIG_PLATFORM_SP7350)
#define PLATFORM_SP7350                 /* Build for SP7350 */
#elif defined(CONFIG_PLATFORM_I143)
#define PLATFORM_I143                   /* Build for I143 */
#endif

/* CSIM build: Enable Stamp. No UART. Less delay. */
#ifdef CONFIG_BOOT_ON_CSIM
#define CSIM_NEW
#endif

/* Zebu build: speed up options */
#if 1 //def CONFIG_BOOT_ON_ZEBU
#define SPEED_UP_UART_BAUDRATE
#define SPEED_UP_SPI_NOR_CLK    /* speed up SPI_NOR flash (eg. Zebu) */
#endif

/* zmem support */
#ifdef CONFIG_USE_ZMEM
#ifdef PLATFORM_I143
#define ZMEM_XBOOT_ADDR    0xA00F0000
#elif defined(CONFIG_PLATFORM_Q645) || defined(CONFIG_PLATFORM_SP7350)
#define ZMEM_XBOOT_ADDR    0xFA200000
#else
#define ZMEM_XBOOT_ADDR    0x1000
#endif
#endif

/***********************
 * xBoot
 ***********************/
#define XBOOT_BUILD           /* define in xboot build */

/**********************
 * Register
 *********************/
#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define REG_BASE           0xF8000000
#define AHB0_REG_BASE      0xF8100000
#else
#define REG_BASE           0x9c000000
#define AHB0_REG_BASE      0x9c100000
#endif

#define RF_GRP(_grp, _reg)              ((((_grp) * 32 + (_reg)) * 4) + REG_BASE)
#define RF_AMBA(_grp, _reg)             ((((_grp) * 1024 + (_reg)) * 4) + REG_BASE)
#define AHB_GRP(_ahb_grp, _grp, _reg)   ((((_grp) * 32 + (_reg)) * 4) + ((_ahb_grp) * 0x1000) + AHB0_REG_BASE)

#ifdef PLATFORM_SP7350
#define REG_BASE_AO           0xF8800000
#define RF_GRP_AO(_grp, _reg)              ((((_grp) * 32 + (_reg)) * 4) + REG_BASE_AO)
#endif

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
#ifdef PLATFORM_SP7350
#define HW_CFG_REG              RF_GRP_AO(0, 31)
#else
#define HW_CFG_REG              RF_GRP(0, 31)
#endif

#define HW_CFG_SHIFT            12
#define HW_CFG_MASK_VAL         0x1F
#define HW_CFG_MASK             (HW_CFG_MASK_VAL << HW_CFG_SHIFT)

#if defined(PLATFORM_Q645)
#define EMMC_BOOT               0x1F
#define SPINAND_BOOT            0x1D
#define USB_ISP                 0x1B
#define SDCARD_ISP              0x19
#define SPI_NOR_BOOT            0x17
#define UART_ISP                0x15
#define AUTO_SCAN               0x11
#define USB_BOOT                0xfd
#define SDCARD_BOOT             0xfe
#define PARA_NAND_BOOT          0xff
#elif defined(PLATFORM_SP7350)
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
#elif defined(PLATFORM_I143)
#define AUTO_SCAN               0x01
#define EMMC_BOOT               0x05
#define SPI_NOR_BOOT            0x07
#define EXT_U54_BOOT            0x09
#define EXT_CA7_BOOT            0x1F
#define INT_CA7_BOOT            0x17
#define SDCARD_ISP              0x11
#define UART_ISP                0x13
#define USB_ISP                 0x15
#define SDCARD_BOOT             0xfb  // not use ,for code compile
#define SPINAND_BOOT            0xfe  // not use ,for code compile
#define PARA_NAND_BOOT          0xfd  // not use ,for code compile
#define EXT_BOOT                0xfc  // not use ,for code compile
#define AUTO_SCAN_ACHIP         0x1F  //for arm ca7,match in start.S ,equal to EXT_CA7_BOOT
#else
#define AUTO_SCAN               0x01
#define AUTO_SCAN_ACHIP         0x15
#define SPI_NOR_BOOT            0x11
#define SPINAND_BOOT            0x09
#define EMMC_BOOT               0x1F
#define EXT_BOOT                0x19
#define SDCARD_ISP              0x07
#define UART_ISP                0x0F
#define USB_ISP                 0x17
#define SDCARD_BOOT             0xfe // add for sdcard boot.
#define PARA_NAND_BOOT          0xff // Q628: no PARA_NAND
#endif

/************************************
 * Secure boot  xboot-->uboot
 ************************************/
#ifdef CONFIG_SECURE_BOOT_SIGN
#ifdef PLATFORM_SPIBAREMETAL
#if defined(PLATFORM_I143)
#define SECURE_VERIFY_FUN_ADDR	(0xF800B000)
#else
#define SECURE_VERIFY_FUN_ADDR	(0x98008001) // function defined in iboot.c
#endif
#else
#if defined(PLATFORM_I143)
#define SECURE_VERIFY_FUN_ADDR	(0xFE00B000)
#else
#define SECURE_VERIFY_FUN_ADDR	(0xFFFF8001)// // function defined in iboot.c
#endif
#endif
#endif

/**********************
 * Clock
 *********************/
#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define XTAL_CLK               (25 * 1000 * 1000)
#define PLLSYS                 (360 * 1000 * 1000)  /* 360 MHz is for CARD_CLK and CARD012_CLK */
#else
#define XTAL_CLK               (27 * 1000 * 1000)
#define PLLSYS                 (202500 * 1000)      /* 202.5MHz */
#endif

/**********************
 * Timer
 *********************/
#define TIMER_KHZ           90

/**********************
 * ROM
 *********************/
#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define BOOT_ROM_BASE       0xfffe8000
#else
#define BOOT_ROM_BASE       0xffff0000
#endif
#define PROTECT_STA_ADDR    0xd000 /* ROM private section */
#define PROTECT_END_ADDR    0xffff /* ROM end */

/**********************
 * SPI
 *********************/
#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define SPI_FLASH_BASE      0xF0000000
#elif defined(PLATFORM_I143)
#define SPI_FLASH_BASE      0xF8000000
#else
#define SPI_FLASH_BASE      0x98000000
#endif
#define SPI_IBOOT_OFFSET    ( 0 * 1024)
#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define SPI_XBOOT_OFFSET    (96 * 1024)
#else
#define SPI_XBOOT_OFFSET    (64 * 1024)
#endif

#define MAGIC_NUM_SPI_BAREMETAL 0x6D622B52

/**********************
 * SRAM
 *********************/
#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define SRAM0_SIZE          (256 * 1024)
#elif defined(PLATFORM_I143)
#define SRAM0_SIZE          (64 * 1024)
#else
#define SRAM0_SIZE          (40 * 1024)
#endif

#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define SRAM0_BASE          0xFA200000
#elif defined(PLATFORM_I143)
#define SRAM0_BASE          0xFE800000
#else
#define SRAM0_BASE          0x9E800000
#endif
#define SRAM0_END           (SRAM0_BASE + SRAM0_SIZE)

#ifdef PLATFORM_I143
#ifdef CONFIG_USE_ZMEM
#define CA7_START_ADDR    (0x200F0000+0x6800+0x20) //ca7 code is offset 26k
#else
#define CA7_START_ADDR    (0x7E800000+0x6800+0x20)
#endif
#endif
#ifdef PLATFORM_I143
#define B_SRAM_BASE_A_VIEW  0xFE800000
#define A_WORK_MEM_BASE     0x9ea00000
#else
#define B_SRAM_BASE_A_VIEW  0x9e800000
#define A_WORK_MEM_BASE     0x9ea00000
#endif

#define A_WORK_MEM_SIZE     (512 * 1024)
#define A_WORK_MEM_END      (A_WORK_MEM_BASE + A_WORK_MEM_SIZE)

/* SRAM layout: must match with boot.ldi */
#if defined(PLATFORM_Q645)
#define XBOOT_BUF_SIZE      (96 * 1024)
#define BOOTINFO_SIZE       (512)
#define GLOBAL_HEADER_SIZE  (512)
#define A64UP_SIZE          (1 * 1024)
#define CDATA_SIZE          (62 * 1024)
#define STORAGE_BUF_SIZE    (32 * 1024)
#define STACK_SIZE          (63 * 1024 - 64)
#define BOOTCOMPT_SIZE      (64)
#define SPACC_RAM_SIZE      (1 * 1024)
#elif defined(PLATFORM_SP7350)
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
#elif defined(PLATFORM_I143)
#if defined(CONFIG_HAVE_SNPS_USB3_DISK)
#define XBOOT_BUF_SIZE      (38 * 1024)
#define STORAGE_BUF_SIZE    (22 * 1024)
#else
#define XBOOT_BUF_SIZE      (27 * 1024)
#define STORAGE_BUF_SIZE    (9 * 1024)
#endif
#define BOOTINFO_SIZE       (384)
#define GLOBAL_HEADER_SIZE  (512)
#define CDATA_SIZE          (128)
#define STACK_SIZE          (3008) /* 3K - 64 */
#elif defined(PLATFORM_Q628) && (CONFIG_PLATFORM_IC_REV < 2)
#define XBOOT_BUF_SIZE      (28 * 1024)
#define STORAGE_BUF_SIZE    (9 * 1024)
#define BOOTINFO_SIZE       (512)
#define GLOBAL_HEADER_SIZE  (512)
#define CDATA_SIZE          (512)
#define STACK_SIZE          (1472) /* 1.5K - 64 */
#else /* new SRAM layout */
#define XBOOT_BUF_SIZE      (27 * 1024)
#define STORAGE_BUF_SIZE    (9 * 1024)
#define BOOTINFO_SIZE       (384)
#define GLOBAL_HEADER_SIZE  (512)
#define CDATA_SIZE          (128)
#define STACK_SIZE          (3008) /* 3K - 64 */
#endif

#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#if defined(__aarch64__)
#define ISB()    do { asm volatile ("isb"); } while (0)
#define DSB()    do { asm volatile ("dsb sy"); } while (0)
#elif defined(__arm__)
#define ISB()    do { asm volatile ("isb"); } while (0)
#define DSB()    do { asm volatile ("dsb"); } while (0)
#endif

#define BOOT_RAM_BASE          SRAM0_BASE
#define XBOOT_A64_ADDR         (BOOT_RAM_BASE + (97 * 1024))

#ifdef CONFIG_USE_ZMEM
#define XBOOT_ADDR       ZMEM_XBOOT_ADDR
#else
#define XBOOT_ADDR       BOOT_RAM_BASE
#endif

#endif

/**********************
 * CPU boot address
 *********************/
#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define CPU_WAIT_A64_VAL             0xfffffffe
#define CORE_CPU_START_POS(core_id)  (CORE0_CPU_START_POS - ((core_id) * 4))
#define CORE3_CPU_START_POS          (0xfa23fc00 - 0x18)  // core3 wait fa23_fbe8
#define CORE2_CPU_START_POS          (0xfa23fc00 - 0x14)  // core2 wait fa23_fbec
#define CORE1_CPU_START_POS          (0xfa23fc00 - 0x10)  // core1 wait fa23_fbf0
#define CORE0_CPU_START_POS          (0xfa23fc00 - 0xc)   // core0 wait fa23_fbf4
#endif

#define CPU_WAIT_INIT_VAL        0xffffffff

#define B_START_POS              (SRAM0_END - 0x8)       // 9e809ff8
#define BOOT_ANOTHER_POS         (SRAM0_END - 0x4)       // 9e809ffc

#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define A_START_POS_B_VIEW       (SRAM0_END - 0xc)
#define A_START_POS_A_VIEW       A_START_POS_B_VIEW
#define BOOT_ANOTHER_POS_A_VIEW  BOOT_ANOTHER_POS
#elif defined(PLATFORM_Q628)
/* B can access A sram */
#define A_START_POS_B_VIEW       (A_WORK_MEM_END - 0xc) // 9ea7fff4 - (core * 4)
#define A_START_POS_A_VIEW       A_START_POS_B_VIEW
#define BOOT_ANOTHER_POS_A_VIEW  BOOT_ANOTHER_POS
#elif defined(PLATFORM_I143)
#define A_BOOT_POS_A_VIEW        0x9e809ffc             // remap to BOOT_ANOTHER_POS
#define A_START_POS_B_VIEW       0x6ea7fff4             // 6ea7fff4 - (core * 4)
#define A_START_POS_A_VIEW       A_START_POS_B_VIEW
#define BOOT_ANOTHER_POS_A_VIEW  BOOT_ANOTHER_POS
#else
/* no A sram */
#define A_START_POS_B_VIEW       (SRAM0_END - 0xc)       // 9e809ff4
#define A_START_POS_A_VIEW       A_START_POS_B_VIEW
#define BOOT_ANOTHER_POS_A_VIEW  BOOT_ANOTHER_POS
#endif


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
* Q645 HSM
***********************/
#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define XB_HSMK_PTR_OFFS  4 // offset to hsmk pointer in xboot.bin
#define XB_HSMK_PTR_ADDR  (XBOOT_ADDR + 32 + XB_HSMK_PTR_OFFS) // 32: xhdr size
#endif

