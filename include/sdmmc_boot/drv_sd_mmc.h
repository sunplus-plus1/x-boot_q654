#ifndef DRV_SD_MMC_H
#define DRV_SD_MMC_H

enum mmc_area {
	MMC_USER_AREA = 0,  /* boot from userpart */
	MMC_BOOT_AREA1 = 1, /* boot from bootpart 1 */
};

int initDriver_SD(int port);

int ReadSDSector(
	unsigned int sectorNo,
	unsigned int pageCount,
	unsigned int* ptrPyldData
);

extern int EMMC_DMA_mode;  /*0: PIO mode, Dram training load IMEM/DMEM bin code into SRAM */
						/*1: DMA mode*/
#endif		//DRV_SD_MMC_H
