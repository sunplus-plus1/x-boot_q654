#include <types.h>
#include <common.h>
#include <regmap.h>
#include "cpio.h"

//#define CPIO_ANA_DBG

#ifdef CPIO_TEST
void cpio_taxi(int tcnt, int ecnt)
{
	unsigned int data_tmp;
	int i;

	prn_string("CPIO Test with FACK AXI begin\n");
	prn_string("Test count: "); prn_dword(tcnt);
	cpior_reg->IO_TCTL &= ~(0xF<<2);
	cpior_reg->IO_TCTL |= (tcnt<<2);
	cpior_reg->IO_TCTL |= 0x1;

	data_tmp = cpior_reg->IO_TCTL;
	while ((data_tmp & 0x1) == 0x0) {
		STC_delay_us(100);
		prn_string("Test run == 0x0\n");
		data_tmp = cpior_reg->IO_TCTL;
	}

	data_tmp = cpior_reg->IO_TCTL;
	if (tcnt == 0x0) {
		STC_delay_1ms (0x100);
		cpior_reg->IO_TCTL &= ~(0x1);
	}

	data_tmp = cpior_reg->IO_TCTL;
	i = 0x10;
	while (((data_tmp & 0x1) != 0x0) && (i != 0x0)) {
		STC_delay_us (100);
		prn_string("Test run == 0x1!!!\n");
		data_tmp = cpior_reg->IO_TCTL;
		i--;
	}
	if (i == 0x0) {
		prn_string("Run bit not cleared after timeout!!!\n");
		prn_dword(cpior_reg->IOP_STS);
		prn_dword(cpior_reg->IO_TCTL);
		while (1);
	}

	data_tmp = cpior_reg->IO_TCTL;
	if ((data_tmp & (0x3F << 11)) != (ecnt << 11)) {
		prn_string("Test error count check fail:\n");
		prn_string("Read Error count: "); prn_dword((data_tmp >> 11) & 0x3F);
		prn_string("Expected Error count: "); prn_dword(ecnt);prn_string("\n");
		prn_dword(cpior_reg->IOP_STS);
		prn_dword(cpior_reg->IO_TCTL);
		while (1);
	} else {
		prn_string("Test passed\n\n");
	}
}

void cpio_taxi_0(void)
{
	int i;

	prn_string("IO test control register="); prn_dword(cpior_reg->IO_TCTL);
	prn_string("Fake AXI test with error insertion\n\n");
	cpior_reg->IO_TCTL |= (0x1<<9);		// Enable error
	cpio_taxi(0xF, 0x1E);
	cpio_taxi(0x0, 0x20);
	prn_string("Fake AXI test without error insertion\n\n");
	cpior_reg->IO_TCTL &= ~(0x1<<9);	// Disable error insert
	cpior_reg->IO_TCTL |= (0x5<<6);		// Set size to random;
	cpio_taxi(0xF, 0x0);
	for (i = 0x0; i < 0x10; i++) {
		cpio_taxi(0x0, 0x0);
	}
}

void cpio_taxi_1(void)
{
	unsigned int data_tmp;
	int i, k;

	prn_string("CPIO Test1 with FACK AXI begin\n");
	cpior_reg->IO_TCTL &= ~(0xF<<2);
	cpior_reg->IO_TCTL |= 0x1;
	data_tmp = cpior_reg->IO_TCTL;
	while ((data_tmp & 0x1) == 0x0) {
		STC_delay_us(100);
		prn_string("Test run == 0x0\n");
		data_tmp = cpior_reg->IO_TCTL;
		while (1);
	}
	data_tmp = cpior_reg->IO_TCTL;
	k = 0x0;
	while ((data_tmp & 0x1) != 0x0) {
		prn_string("\nTimes "); prn_dword0 (k); prn_string(" Test running... ...\n");
		prn_string("Test ctrl: "); prn_dword(cpior_reg->IO_TCTL);
		prn_string("PHY status: "); prn_dword(cpior_reg->IOP_STS);
		data_tmp = cpior_reg->IO_TCTL;
		prn_string("Error Count: "); prn_dword((data_tmp >> 11) & 0x3F);
		if (((data_tmp >> 11) & 0x3F) == 0x20) {
			prn_string("Test hang-up!!!\n");
			while (1);
		}
		for (i = 0; i < 0x6; i++) {
			STC_delay_1ms (0x100);
		}
		k++;
	}
	prn_string("Test stoped by error\n");
	while (1);
}

void cpio_cfg_pll(void)
{
	prn_string("Config CPIO PLL and analog\n");

#if 0
	prn_string("VCO: 3.2G, PLL: 0.4G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xFF330823;
	cpior_reg->AFE_CTL[7] = 0x00005008;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xDF330823;
	cpior_reg->AFE_CTL[7] = 0x00005008;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 1.6G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xBF330823;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 3.2G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0x9F330823;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 4.1G, PLL: 4.1G\n");
	cpior_reg->AFE_CTL[0] = 0x248083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDF9;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x9F330A63;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 1.25G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90081000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xDF330CA3;
	cpior_reg->AFE_CTL[7] = 0x00005008;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 2.5G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90081000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xBF330CA3;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 1
	prn_string("VCO: 6.4G, PLL: 6.4G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2520013B;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x92C18833;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 7.4G, PLL: 7.4G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDFB;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x92C18973;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 9.6G, PLL: 2.4G\n");
	cpior_reg->AFE_CTL[0] = 0x248183FE;
	cpior_reg->AFE_CTL[1] = 0x4527BDF9;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0xD2C18C33;
	cpior_reg->AFE_CTL[7] = 0x00007008;
#endif
#if 0
	prn_string("VCO: 9.6G, PLL: 4.8G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x252001FB;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0xB2C18C33;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 9.6G, PLL: 9.6G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDFB;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x92C18C33;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 10G, PLL: 10G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDFB;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x9F330CB3;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 12.6G, PLL: 12.6G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDFB;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x9F330FF3;
	cpior_reg->AFE_CTL[7] = 0x00004000;
#endif

	prn_dword(cpior_reg->AFE_CTL[6]);
}

void cpio_reset(int re_cfg)
{
	prn_string("Reset CPIO controller\n");

	MOON0_REG_AO->reset[1] = ((0x2<<16) | 0x2);	// CPIOR_RESET = 1
	STC_delay_us (1);
	MOON0_REG_AO->reset[1] = (0x2<<16);		// CPIOR_RESET = 0
	STC_delay_us (1);

	cpior_reg->AFE_CTL[7] |= 0x1;		// enable test clock output
	prn_dword(cpior_reg->PHY_CTRL);
	cpior_reg->PHY_CTRL = (0x1<<19);	// Disable auto initial;
	//cpior_reg->PHY_CTRL = (0x1<<18);	// 2 lane mode
	cpior_reg->PHY_CTRL |= ((0x1<<5) | (0x1<<(5+16)));	// internal pad

	if (re_cfg) {
		cpio_cfg_pll();
	}

	//cpior_reg->AFE_CTL[7] |= (0x1<<8);	// GPO DS;
	//cpior_reg->AFE_CTL[7] |= (0x1<<9);	// GPIO inv

	//cpior_reg->TEST_C[1] |= (0x1<<9);	// force control
	//cpior_reg->AFE_CTL[0] |= (0x1F<<10);	// XOVER all
	//cpior_reg->AFE_CTL[0] |= (0x1F<<17);	// POL-inv all

	cpior_reg->PHY_CTRL |= (0x1<<7);	// Release ref reset
	prn_dword(cpior_reg->PHY_CTRL);
	STC_delay_us (100);
}

void cpio_reset1(int dly0, int dly1)
{
	prn_string("Reset CPIO controller"); prn_dword0 (dly0); prn_dword(dly1);

	MOON0_REG_AO->reset[1] = ((0x2<<16) | 0x2);	// CPIOR_RESET = 1
	STC_delay_us (1);
	MOON0_REG_AO->reset[1] = (0x2<<16);		// CPIOR_RESET = 0
	STC_delay_us (1);

	cpior_reg->AFE_CTL[7] |= 0x1;		// enable test clock output
	prn_dword(cpior_reg->PHY_CTRL);
	cpior_reg->PHY_CTRL = (0x1<<19);	// Disable auto initial;
	//cpior_reg->PHY_CTRL = (0x1<<18);	// 2 lane mode
	//cpior_reg->PHY_CTRL |= ((0x1<<5) | (0x1<<(5+16)));	// internal pad

	cpio_cfg_pll();
	//cpior_reg->AFE_CTL[0] |= (0x1<<17);	// POL_INV
	//cpior_reg->AFE_CTL[0] |= (0x1<<30);	// RCK dly en1
	//cpior_reg->AFE_CTL[0] |= (dly0<<30);	// RCK dly en2
	//cpior_reg->AFE_CTL[0] |= (dly1<<12);	// RCK dly value

	if (dly0 == 0x0) {
		cpior_reg->AFE_CTL[3] &= ~(0x7<<26);
		cpior_reg->AFE_CTL[3] |= ((dly1&0x7)<<26);
		cpior_reg->AFE_CTL[4] &= ~(0x7);
		cpior_reg->AFE_CTL[4] |= (dly1&0x7);
	} else {
		cpior_reg->AFE_CTL[3] &= ~(0x7<<29);
		cpior_reg->AFE_CTL[3] |= ((dly1&0x7)<<29);
		cpior_reg->AFE_CTL[4] &= ~(0x7<<3);
		cpior_reg->AFE_CTL[4] |= ((dly1&0x7)<<3);
	}

	//cpior_reg->AFE_CTL[7] |= (0x1<<8);	// GPO DS;
	//cpior_reg->AFE_CTL[7] |= (0x1<<9);	// GPIO inv

	//cpior_reg->TEST_C[1] |= (0x1<<9);	// force control
	//cpior_reg->AFE_CTL[0] |= (0x1F<<10);	// XOVER all
	//cpior_reg->AFE_CTL[0] |= (0x1F<<17);	// POL-inv all

	cpior_reg->PHY_CTRL |= (0x1<<7);	// Release ref reset
	prn_dword(cpior_reg->PHY_CTRL);
	STC_delay_us (100);
}

void ana_test(int xover)
{
	int ecnt, ecnt0, ecnt1, ecnt2, ecnt3;
	int i;

	prn_string("Analog function TEST\n");
	cpior_reg->TEST_C[1] |= (0x1<<8);
	//cpior_reg->TEST_C[0] |= (0x1<<4);

	//cpior_reg->TEST_D[0] = 0xe1e1e1e1;
	//cpior_reg->TEST_D[1] = 0xeeeeeeee;
	//cpior_reg->TEST_D[1] = 0x815FA815;
	//cpior_reg->TEST_D[1] = 0xF3CF3CF0;
	//cpior_reg->TEST_D[2] = 0x11111111;
	//cpior_reg->TEST_D[2] = ~(0x815FA815);
	//cpior_reg->TEST_D[2] = ~(0xF3CF3CF0);
	//cpior_reg->TEST_D[3] = 0xF3CF3CF0;
	cpior_reg->TEST_D[0] = 0x55555555;
	cpior_reg->TEST_D[1] = 0x55555555;
	cpior_reg->TEST_D[2] = 0x55555555;
	cpior_reg->TEST_D[3] = 0x55555555;
	//cpior_reg->TEST_D[0] = 0x815FA815;
	//cpior_reg->TEST_D[1] = 0x815FA815;
	//cpior_reg->TEST_D[2] = 0x815FA815;
	//cpior_reg->TEST_D[3] = 0x815FA815;

#if 0
	prn_string("VCO: 3.2G, PLL: 0.4G\n");
	cpior_reg->AFE_CTL[0] = 0x240083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xFF330823;
	cpior_reg->AFE_CTL[7] = 0x00005008;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FF;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xDF330823;
	cpior_reg->AFE_CTL[7] = 0x00005008;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 1.6G\n");
	cpior_reg->AFE_CTL[0] = 0x240083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xBF330823;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 3.2G\n");
	cpior_reg->AFE_CTL[0] = 0x240083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0x9F330823;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 4.1G, PLL: 4.1G\n");
	cpior_reg->AFE_CTL[0] = 0x248083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDF9;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x9F330A63;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 1.25G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FF;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90081000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xDF330CA3;
	cpior_reg->AFE_CTL[7] = 0x00005008;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 2.5G\n");
	cpior_reg->AFE_CTL[0] = 0x240083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90081000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xBF330CA3;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 5.0G\n");
	cpior_reg->AFE_CTL[0] = 0x240083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90081000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0x9F330CA3;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 6.4G, PLL: 6.4G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2520013B;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x92C18833;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 7.4G, PLL: 7.4G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDFB;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x92C18973;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 9.6G, PLL: 2.4G\n");
	cpior_reg->AFE_CTL[0] = 0x248083FA;
	cpior_reg->AFE_CTL[1] = 0x4527BDF9;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0xD2C18C33;
	cpior_reg->AFE_CTL[7] = 0x00007008;
#endif
#if 1
	prn_string("VCO: 9.6G, PLL: 4.8G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDFB;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0xB2C18C33;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 9.6G, PLL: 9.6G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDFB;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x92C18C33;
	cpior_reg->AFE_CTL[7] = 0x00007000;
#endif
#if 0
	prn_string("VCO: 12.6G, PLL: 12.6G\n");
	cpior_reg->AFE_CTL[0] = 0x2C8083FA;
	cpior_reg->AFE_CTL[1] = 0x2527BDFB;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0x9F330FF3;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif

	if (xover) {
		cpior_reg->AFE_CTL[0] |= (0x1F<<10);
	}

	prn_string("Print All AFE_CTL register\n");
	for (i = 0; i < 8; i++) {
		prn_dword(cpior_reg->AFE_CTL[i]);
	}
	prn_string("Test RX Datas\n");
	ecnt = 0;

	prn_string("\nTest Channel0\n");
	cpior_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpior_reg->AFE_CTL[0] |= (0x11<<5);	// Enable channel0
	ecnt0 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpior_reg->TEST_D[4]);
		prn_dword(cpior_reg->TEST_D[5]);
#endif
		if ((cpior_reg->TEST_D[4] != 0xaaaaaaaa) && (cpior_reg->TEST_D[4] != 0x55555555))
			ecnt0++;
		if (cpior_reg->TEST_D[5] != 0x0)
			ecnt1++;
	}
	if (ecnt0 != 0x0) {
		prn_string("Channel0 Test Fail with error count: "); prn_dword(ecnt0);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	prn_string("\nTest Channel1\n");
	cpior_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpior_reg->AFE_CTL[0] |= (0x12<<5);	// Enable channel1
	ecnt1 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpior_reg->TEST_D[4]);
		prn_dword(cpior_reg->TEST_D[5]);
#endif
		if (cpior_reg->TEST_D[4] != 0x0)
			ecnt1++;
		if ((cpior_reg->TEST_D[5] != 0xaaaaaaaa) && (cpior_reg->TEST_D[5] != 0x55555555))
			ecnt1++;
	}
	if (ecnt1 != 0x0) {
		prn_string("Channel1 Test Fail with error count: "); prn_dword(ecnt1);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	cpior_reg->TEST_C[0] |= (0x1<<4);

	prn_string("\nTest Channel2\n");
	cpior_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpior_reg->AFE_CTL[0] |= (0x14<<5);	// Enable channel2
	ecnt2 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpior_reg->TEST_D[4]);
		prn_dword(cpior_reg->TEST_D[5]);
#endif
		if ((cpior_reg->TEST_D[4] != 0xaaaaaaaa) && (cpior_reg->TEST_D[4] != 0x55555555))
			ecnt2++;
		if (cpior_reg->TEST_D[5] != 0x0)
			ecnt2++;
	}
	if (ecnt2 != 0x0) {
		prn_string("Channel2 Test Fail with error count: "); prn_dword(ecnt2);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	prn_string("\nTest Channel3\n");
	cpior_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpior_reg->AFE_CTL[0] |= (0x18<<5);	// Enable channel3
	ecnt3 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpior_reg->TEST_D[4]);
		prn_dword(cpior_reg->TEST_D[5]);
#endif
		if (cpior_reg->TEST_D[4] != 0x0)
			ecnt3++;
		if ((cpior_reg->TEST_D[5] != 0xaaaaaaaa) && (cpior_reg->TEST_D[5] != 0x55555555))
			ecnt3++;
	}
	if (ecnt3 != 0x0) {
		prn_string("Channel1 Test Fail with error count: "); prn_dword(ecnt3);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	prn_string("\nTest 2lane mode transfers(lane1, lane2)\n");
	cpior_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpior_reg->AFE_CTL[0] |= (0x16<<5);	// Enable channel1, channel2
	ecnt1 = 0;
	ecnt2 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpior_reg->TEST_D[4]);
		prn_dword(cpior_reg->TEST_D[5]);
#endif
		if ((cpior_reg->TEST_D[4] != 0xaaaaaaaa) && (cpior_reg->TEST_D[4] != 0x55555555))
			ecnt2++;
		if (cpior_reg->TEST_D[5] != 0x0)
			ecnt2++;
	}
	cpior_reg->TEST_C[0] &= ~(0x3<<4);
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpior_reg->TEST_D[4]);
		prn_dword(cpior_reg->TEST_D[5]);
#endif
		if (cpior_reg->TEST_D[4] != 0x0)
			ecnt1++;
		if ((cpior_reg->TEST_D[5] != 0xaaaaaaaa) && (cpior_reg->TEST_D[5] != 0x55555555))
			ecnt1++;
	}
	if ((ecnt1 != 0x0) || (ecnt2 != 0x0)) {
		prn_string("2lane mode Test Fail with error count (ch1, ch2): "); prn_dword0 (ecnt1); prn_string(" , "); prn_dword(ecnt2);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	prn_string("\nTest 4lane mode transfers(lane0, lane1, lane2, lane3)\n");
	cpior_reg->AFE_CTL[0] &= ~(0x1F<<5);
	cpior_reg->AFE_CTL[0] |= (0x1F<<5);	// Enable channel0, channel1, channel2, channel3
	ecnt0 = 0;
	ecnt1 = 0;
	ecnt2 = 0;
	ecnt3 = 0;
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpior_reg->TEST_D[4]);
		prn_dword(cpior_reg->TEST_D[5]);
#endif
		if ((cpior_reg->TEST_D[4] != 0xaaaaaaaa) && (cpior_reg->TEST_D[4] != 0x55555555))
			ecnt0++;
		if ((cpior_reg->TEST_D[5] != 0xaaaaaaaa) && (cpior_reg->TEST_D[5] != 0x55555555))
			ecnt1++;
	}
	cpior_reg->TEST_C[0] |= (0x1<<4);
	for (i = 0; i < 0x1000; i++) {
#ifdef CPIO_ANA_DBG
		prn_dword(cpior_reg->TEST_D[4]);
		prn_dword(cpior_reg->TEST_D[5]);
#endif
		if ((cpior_reg->TEST_D[4] != 0xaaaaaaaa) && (cpior_reg->TEST_D[4] != 0x55555555))
			ecnt2++;
		if ((cpior_reg->TEST_D[5] != 0xaaaaaaaa) && (cpior_reg->TEST_D[5] != 0x55555555))
			ecnt3++;
	}
	if ((ecnt0 != 0x0) || (ecnt1 != 0x0) || (ecnt2 != 0x0) || (ecnt3 != 0x0)) {
		prn_string("4lane mode Test Fail with error count (ch0, ch1, ch2, ch3): "); prn_dword0 (ecnt0);
		prn_string(" , "); prn_dword0 (ecnt1); prn_string(" , "); prn_dword0 (ecnt2); prn_string(" , "); prn_dword(ecnt3);
		ecnt++;
	} else
		prn_string("Test Pass\n");

	if (ecnt != 0x0) {
		prn_string("\nAnalog Test with "); prn_dword0(ecnt); prn_string(" test Fail!!\n");
#ifdef CPIO_ANA_DBG
		while (1);
#endif
	} else
		prn_string("\nAll Analog Test Pass\n");
	//while (1);
}

void ana_test1(int re_cfg, int cfg_v, int cfg_v0, int cfg_v1, int cfg_v2, int cfg_v3)
{
	int ecnt, ecnt0, ecnt1, ecnt2, ecnt3;
	unsigned int tmp_data0[10];
	unsigned int tmp_data1[10];
	unsigned int tmp_data2[10];
	unsigned int tmp_data3[10];
	unsigned int tmp_data;
	int i;

	prn_string("Analog function TEST\n");
	cpior_reg->TEST_C[1] |= (0x1<<8);
	//cpior_reg->TEST_C[0] |= (0x1<<4);

	//cpior_reg->TEST_D[0] = 0xe1e1e1e1;
	cpior_reg->TEST_D[0] = 0x815FA815;
	cpior_reg->TEST_D[1] = 0x815FA815;
	cpior_reg->TEST_D[2] = 0x815FA815;
	cpior_reg->TEST_D[3] = 0x815FA815;
	//cpior_reg->TEST_D[2] = ~(0xF3CF3CF0);
	//cpior_reg->TEST_D[3] = 0xF3CF3CF0;

	//cpior_reg->TEST_D[0] = 0xAAAAAAAA;
	//cpior_reg->TEST_D[1] = 0xAAAAAAAA;
	//cpior_reg->TEST_D[2] = 0xAAAAAAAA;
	//cpior_reg->TEST_D[3] = 0xAAAAAAAA;

#if 0
	prn_string("VCO: 3.2G, PLL: 0.4G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xFF330823;
	cpior_reg->AFE_CTL[7] = 0x00005008;
#endif
#if 1
	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FF;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xDF330823;
	cpior_reg->AFE_CTL[7] = 0x00005008;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 1.6G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xBF330823;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 2.5G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90081000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xBF330CA3;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 1.25G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90081000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xDF330CA3;
	cpior_reg->AFE_CTL[7] = 0x00005008;
#endif

#if 0
	cpior_reg->AFE_CTL[0] |= (0x1F<<10);	// XOVER mode all
#endif
#if 1
	cpior_reg->AFE_CTL[0] &= ~(0x1<<15);	// Disable analog loopback
#endif

	if (re_cfg == 0x1) {
		cpior_reg->AFE_CTL[5] &= ~(0x7<<9);
		cpior_reg->AFE_CTL[5] |= ((cfg_v&0x7)<<9);

		cpior_reg->AFE_CTL[5] &= ~(0x7<<13);
		cpior_reg->AFE_CTL[5] |= ((cfg_v0&0x7)<<13);

		cpior_reg->AFE_CTL[3] &= ~(0x7<<26);
		cpior_reg->AFE_CTL[3] |= ((cfg_v1&0x7)<<26);
		cpior_reg->AFE_CTL[4] &= ~(0x7);
		cpior_reg->AFE_CTL[4] |= (cfg_v1&0x7);

		cpior_reg->AFE_CTL[3] &= ~(0x7<<29);
		cpior_reg->AFE_CTL[3] |= ((cfg_v2&0x7)<<29);
		cpior_reg->AFE_CTL[4] &= ~(0x7<<3);
		cpior_reg->AFE_CTL[4] |= ((cfg_v2&0x7)<<3);

		cpior_reg->AFE_CTL[0] |= ((cfg_v3&0x20)<<12);	// POL_INV
		cpior_reg->AFE_CTL[0] |= (0x1<<30);		// RCK dly en1
		cpior_reg->AFE_CTL[1] |= ((cfg_v3&0x10)<<26);	// RCK dly en2
		cpior_reg->AFE_CTL[2] |= (cfg_v3<<12);		// RCK dly value
	}

	prn_string("Print All AFE_CTL register\n");
	for (i = 0; i < 8; i++) {
		prn_dword(cpior_reg->AFE_CTL[i]);
	}

	for (i = 0; i < 10; i++) {
		tmp_data0[i] = cpior_reg->TEST_D[4];
	}
	for (i = 0; i < 10; i++) {
		tmp_data1[i] = cpior_reg->TEST_D[5];
	}

	cpior_reg->TEST_C[0] |= (0x1<<4);

	for (i = 0; i < 10; i++) {
		tmp_data2[i] = cpior_reg->TEST_D[4];
	}
	for (i = 0; i < 10; i++) {
		tmp_data3[i] = cpior_reg->TEST_D[5];
	}

	prn_string("\nPrint TX datas\n");
	prn_string("CH0 data: "); prn_dword(cpior_reg->TEST_D[0]);
	prn_string("CH1 data: "); prn_dword(cpior_reg->TEST_D[1]);
	prn_string("CH2 data: "); prn_dword(cpior_reg->TEST_D[2]);
	prn_string("CH3 data: "); prn_dword(cpior_reg->TEST_D[3]);

	prn_string("\nPrint RX datas\n");
	prn_string("CH0 data:\n");
	for (i = 0; i < 10; i++) {
		prn_dword(tmp_data0[i]);
	}
	prn_string("CH1 data:\n");
	for (i = 0; i < 10; i++) {
		prn_dword(tmp_data1[i]);
	}
	prn_string("CH2 data:\n");
	for (i = 0; i < 10; i++) {
		prn_dword(tmp_data2[i]);
	}
	prn_string("CH3 data:\n");
	for (i = 0; i < 10; i++) {
		prn_dword(tmp_data3[i]);
	}

	ecnt0 = 0x0;
	ecnt1 = 0x0;
	ecnt2 = 0x0;
	ecnt3 = 0x0;
	for (i = 0; i < 10; i++) {
		if (tmp_data0[i] != tmp_data0[0])
			ecnt0++;
		if (tmp_data1[i] != tmp_data1[0])
			ecnt1++;
		if (tmp_data2[i] != tmp_data2[0])
		ecnt2++;
	if (tmp_data3[i] != tmp_data3[0])
		ecnt3++;
	}

	if (ecnt0 != 0x0)
		prn_string("CH0 all data not equal\n");
	if (ecnt1 != 0x0)
		prn_string("CH1 all data not equal\n");
	if (ecnt2 != 0x0)
		prn_string("CH2 all data not equal\n");
	if (ecnt3 != 0x0)
		prn_string("CH3 all data not equal\n");

	ecnt = 0x1;
	for (i = 0; i < 32; i++) {
		tmp_data = (tmp_data0[0]>>i) | (tmp_data0[0]<<(32-i));
		if (tmp_data == cpior_reg->TEST_D[0]) {
			ecnt = 0x0;
			break;
		}
		//prn_dword(tmp_data);
	}
	if (ecnt == 0x1) {
		prn_string("CH0 RX data check fail\n");
	}
	ecnt0 += ecnt;
	if (ecnt0 != 0x0) {
		prn_string("CH0 test with error count: "); prn_dword(ecnt0);
	} else {
		prn_string("CH0 test pass\n");
	}

	ecnt = 0x1;
	for (i = 0; i < 32; i++) {
		tmp_data = (tmp_data1[0]>>i) | (tmp_data1[0]<<(32-i));
		if (tmp_data == cpior_reg->TEST_D[1]) {
			ecnt = 0x0;
			break;
		}
		//prn_dword(tmp_data);
	}
	if (ecnt == 0x1) {
		prn_string("CH1 RX data check fail\n");
	}
	ecnt1 += ecnt;
	if (ecnt1 != 0x0) {
		prn_string("CH1 test with error count: "); prn_dword(ecnt1);
	} else {
		prn_string("CH1 test pass\n");
	}

	ecnt = 0x1;
	for (i = 0; i < 32; i++) {
		tmp_data = (tmp_data2[0]>>i) | (tmp_data2[0]<<(32-i));
		if (tmp_data == cpior_reg->TEST_D[2]) {
			ecnt = 0x0;
			break;
		}
		//prn_dword(tmp_data);
	}
	if (ecnt == 0x1) {
		prn_string("CH2 RX data check fail\n");
	}
	ecnt2 += ecnt;
	if (ecnt2 != 0x0) {
		prn_string("CH2 test with error count: "); prn_dword(ecnt2);
	} else {
		prn_string("CH2 test pass\n");
	}

	ecnt = 0x1;
	for (i = 0; i < 32; i++) {
		tmp_data = (tmp_data3[0]>>i) | (tmp_data3[0]<<(32-i));
		if (tmp_data == cpior_reg->TEST_D[3]) {
			ecnt = 0x0;
			break;
		}
		//prn_dword(tmp_data);
	}
	if (ecnt == 0x1) {
		prn_string("CH3 RX data check fail\n");
	}
	ecnt3 += ecnt;
	if (ecnt3 != 0x0) {
		prn_string("CH3 test with error count: "); prn_dword(ecnt3);
	} else {
		prn_string("CH3 test pass\n");
	}
}

void gpo_test(int tnum)
{
	prn_string("GPO test set\n");
	cpior_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | (tnum<<4) | 0x5);
	while ((cpior_reg->TEST_C[1]) & (0x1 << 31));

	prn_string("GPO test run\n");
	cpior_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | 0x5);
	while ((cpior_reg->TEST_C[1]) & (0x1 << 31));

	if ((cpior_reg->IOP_STS) & (0xF << 20)) {
		prn_string("Test Fail!!!\n");
		prn_dword(cpior_reg->IOP_STS);
	} else {
		prn_string("Test Pass!!\n");
	}
}

void gpo_test1(void)
{
	int i;

	prn_dword(cpior_reg->PHY_CTRL);
	prn_string("GPO test for clock\n");
	gpo_test(1);

	prn_string("GPO test for valid\n");
	gpo_test(3);

	prn_string("GPO test for ready\n");
	gpo_test(4);

	for (i = 0; i < 0x8; i++) {
		prn_string("GPO test for data "); prn_dword(i);
		gpo_test(i | 0x8);
	}

	prn_string("GPO test for all pads\n");
	gpo_test(2);
	while (1) {
		cpior_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | 0x5);
		while ((cpior_reg->TEST_C[1]) & (0x1 << 31));
	}
}

void cpio_test(void)
{
#if 0
	prn_dword(cpior_reg->PHY_CTRL);
	while (1);
#endif

	unsigned int data_tmp;
	int i;

	prn_string("\n\n--- CPIO-R test Begin ----\n\n");
	MOON4_REG_AO->sft_cfg[23] = 0x00080000;		// EN_MIPI0_RX = 0
#if 0 // Enable CPIOR_CLK
	MOON1_REG_AO->sft_cfg[1]  = 0x02000000;		// GMAC_SEL_SEL = 0
	MOON4_REG_AO->sft_cfg[27] = 0x007F0002;		// Select CPIOR_CLK to CLK_PROBE
	MOON1_REG_AO->sft_cfg[10] = 0x00100010;  	// PROBE_PORT_SEL = 1
	cpior_reg->AFE_CTL[7] |= 0x1;			// enable test clock output
#endif

	data_tmp = MOON0_REG_AO->reset[1];
	if ((data_tmp & 0x2) != 0x0) {
		prn_string("CPIO system reset not released, releasing!!\n");
		MOON0_REG_AO->reset[1] = (0x2<<16);	// CPIOR_RESET = 0
		STC_delay_us (100);
	}

	//cpior_reg->PHY_CTRL = (0x1<<19);			// Disable auto initial;
	//cpior_reg->PHY_CTRL = (0x1<<6);
	//cpior_reg->PHY_CTRL |= ((0x1<<5) | (0x1<<(5+16)));	// internal pad

	//cpior_reg->AFE_CTL[1] |= (0x3<<8);
	cpior_reg->AFE_CTL[7] |= (0x1<<8);	// GPO DS;
	cpior_reg->AFE_CTL[7] |= (0x1<<9);

	//cpior_reg->TEST_C[1] |= (0x1<<9);	// force control
	//cpior_reg->AFE_CTL[0] |= (0x1F<<10);	// XOVER all

	cpior_reg->PHY_CTRL |= (0x1<<7); 	// Release ref reset

	STC_delay_us(100);

	//gpo_test1();

#if 0
	cpio_reset(1);
	prn_string("ATPG Test\n\n");
	cpior_reg->IO_TCTL |= (0x3<<18);
	STC_delay_us (1);
	cpio_taxi_0();
	STC_delay_us (100);

	#if 0
	prn_dword(cpior_reg->PHY_CTRL);
	prn_string("GPO test set\n");
	cpior_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | (0x2<<4) | 0x5);
	while ((cpior_reg->TEST_C[1]) & (0x1 << 31));
	prn_string("GPIO test run\n");
	cpior_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | 0x5);
	while (1) {
		while ((cpior_reg->TEST_C[1]) & (0x1 << 31));
		cpior_reg->TEST_C[1] = ((0x1<<31) | (0x8<<12) | 0x5);
		//prn_string("GPO test\n");
	}
	#endif
	cpior_reg->TEST_C[0] |= ((0x1<<6) | (0xF));
	cpior_reg->TEST_C[0] |= (0x1<<31);
	prn_dword(cpior_reg->TEST_C[0]);
	prn_dword(cpior_reg->IOP_STS);
	prn_dword(cpior_reg->TEST_D[4]);
	prn_dword(cpior_reg->TEST_D[5]);
	prn_string("TEST result\n");
	prn_dword(cpior_reg->TEST_C[0]);
	prn_dword(cpior_reg->IOP_STS);
	prn_dword(cpior_reg->TEST_D[4]);
	prn_dword(cpior_reg->TEST_D[5]);
	STC_delay_1ms (10);
	cpior_reg->TEST_C[0] &= ~(0x1<<31);
	prn_string("TEST result\n");
	cpior_reg->TEST_C[0] &= ~(0x3<<4);
	prn_dword(cpior_reg->TEST_C[0]);
	prn_dword(cpior_reg->IOP_STS);
	prn_dword(cpior_reg->TEST_D[4]);
	prn_dword(cpior_reg->TEST_D[5]);
	prn_string("TEST result\n");
	cpior_reg->TEST_C[0] |= (0x1<<4);
	prn_dword(cpior_reg->TEST_C[0]);
	prn_dword(cpior_reg->IOP_STS);
	prn_dword(cpior_reg->TEST_D[4]);
	prn_dword(cpior_reg->TEST_D[5]);
	prn_string("TEST result\n");
	cpior_reg->TEST_C[0] &= ~(0x3<<4);
	cpior_reg->TEST_C[0] |= (0x2<<4);
	prn_dword(cpior_reg->TEST_C[0]);
	prn_dword(cpior_reg->IOP_STS);
	prn_dword(cpior_reg->TEST_D[4]);
	prn_dword(cpior_reg->TEST_D[5]);
	prn_string("TEST result\n");
	cpior_reg->TEST_C[0] |= (0x3<<4);
	prn_dword(cpior_reg->TEST_C[0]);
	prn_dword(cpior_reg->IOP_STS);
	prn_dword(cpior_reg->TEST_D[4]);
	prn_dword(cpior_reg->TEST_D[5]);
	while (1);
#endif

#if 0
	ana_test1(0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
	while (1);

	int j, k, m, n;

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			for (k = 0; k < 8; k++) {
				for (m = 0; m < 8; m++) {
					for (n = 0; n < 0x40; n++) {
						prn_string("\nTX_ADJ: "); prn_dword0(i);prn_string(" RX_ADJ: "); prn_dword(j);
						prn_string("EQ0_TC: "); prn_dword0(k);prn_string(" EQ1_TC: "); prn_dword(m);
						prn_string("RX_DLY: "); prn_dword(n);
						ana_test1(0x1, i, j, k, m, n);
					}
				}
			}
		}
	}

	while (1);
	//ana_test(1);
	//STC_delay_1ms (1);
	//prn_string("Enable XOVER mode\n");
	//ana_test(1);
	//while (1);
#endif
	//ana_test(0);
	//while (1);
#if 0
	cpio_reset(0x0);

	prn_string("Enable Controller loopback in low speed\n\n");
	cpior_reg->IO_TCTL |= (0x1<<18);
	STC_delay_us (1);
	cpio_taxi_0();

	prn_string("Enable Controller loopback in high speed\n\n");
	cpior_reg->IO_TCTL |= (0x7<<18);
	STC_delay_us (1);
	cpio_taxi_0();
#endif

#if 0
	cpio_reset(0x1);

	prn_string("Enable DPHY loopback in low speed\n\n");
	cpior_reg->IO_TCTL |= (0x2<<18);
	STC_delay_us (1);
	cpio_taxi_0();

	prn_string("Switch to high speed in DPHY loopback mode\n");
	cpior_reg->PHY_CTRL |= 0x1;
	STC_delay_us (100);

	data_tmp = cpior_reg->PHY_CTRL;
	if ((data_tmp & 0x1) == 0x0) {
		prn_string("Mode Not switched to high speed mode\n");
		prn_dword(cpior_reg->PHY_CTRL);
		prn_dword(cpior_reg->IOP_STS);
		while (1);
	}

	data_tmp = cpior_reg->IOP_STS;
	if (((data_tmp & 0x1) == 0x0) || ((data_tmp & (0x1<<5)) != 0x0) || ((data_tmp & (0x1<<2)) != 0x0)) {
		prn_string("PHY Status check fail\n");
		prn_dword(cpior_reg->IOP_STS);
		while (1);
	}
	prn_string("Mode switch pass\n");
	STC_delay_us (1);

	cpio_taxi_0();

	prn_string("All Digital Loopback Test Finished!!!\n\n");
#endif

	cpio_reset(0x1);
//	for (k = 0; k < 0x20; k++) {
//	cpio_reset1(((k>>4) & 0x1), (k&0xF));
//	prn_string("\n");
//	prn_dword(k);
	prn_string("Enable APHY loopback in low speed\n\n");
	cpior_reg->IO_TCTL |= (0x3<<18);
	//cpior_reg->IO_TCTL |= (0x4<<18);
	STC_delay_us (1);
	cpio_taxi_0();
	STC_delay_us (5);
	prn_dword(cpior_reg->AFE_CTL[5]);

	prn_string("Switch to high speed in APHY loopback mode\n");
	cpior_reg->PHY_CTRL |= 0x1;
	//cpior_reg->PHY_CTRL |= ((0x1<<5) | (0x1<<(5+16)));
	STC_delay_us (100);

	data_tmp = cpior_reg->PHY_CTRL;
	if ((data_tmp & 0x1) == 0x0) {
		prn_string("Mode Not switched to high speed mode\n");
		prn_dword(cpior_reg->PHY_CTRL);
		prn_dword(cpior_reg->IOP_STS);
		cpior_reg->PHY_CTRL |= 0x1;
		STC_delay_us (100);
		prn_dword(cpior_reg->PHY_CTRL);
		prn_dword(cpior_reg->IOP_STS);
		prn_string("Debug Registers\n");
		for (i = 0; i < 8; i++) {
			prn_dword(cpior_reg->IO_DBG[i]);
		}
		//continue;
		while (1);
	}

	data_tmp = cpior_reg->IOP_STS;
	if (((data_tmp & 0x1) == 0x0) || ((data_tmp & (0x1<<5)) != 0x0) || ((data_tmp & (0x1<<2)) != 0x0)) {
		prn_string("PHY Status check fail\n");
		prn_dword(cpior_reg->PHY_CTRL);
		prn_dword(cpior_reg->IOP_STS);
		prn_string("Debug Registers\n");
		for (i = 0; i < 8; i++) {
			prn_dword(cpior_reg->IO_DBG[i]);
		}
		//continue;
		while (1);
	}
	prn_string("Mode switch pass\n");
	prn_dword(cpior_reg->PHY_CTRL);
	STC_delay_us (1);
//	}
//	while (1);

	//while (1) {
	//	cpio_taxi_0();
	//}
	cpio_taxi_0();

	//cpio_taxi_1();

	prn_string("Loopback Test Finished!!!\n\n");
	while (1);
}
#endif // CPIO_TEST

void cpio_slave(void)
{
	int dly_cnt0, dly_cnt1;
	unsigned int data_tmp;

	prn_string("\n\n---- CPIO-R slave mode Begin ----\n\n");
	MOON4_REG_AO->sft_cfg[23] = 0x00080000;		// EN_MIPI0_RX = 0

	data_tmp = MOON0_REG_AO->reset[1];
	if ((data_tmp & 0x2) != 0x0) {
		prn_string("CPIO system reset not released, releasing!!\n");
		MOON0_REG_AO->reset[1] = (0x2<<16);	// CPIOR_RESET = 0
		STC_delay_us (100);
	}

	// Change SN
	cpior_reg->IO_SNL = 0x0000A5AA;
	cpior_reg->IO_SNH = 0x00005A5A;

#if 0
	prn_string("VCO: 3.2G, PLL: 0.4G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xFF330823;	// AFE_CTL6, POSTDIV = 3, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x20, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xDF330823;	// AFE_CTL6, POSTDIV = 2, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x20, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 1
	prn_string("VCO: 4.0G, PLL: 1.0G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xDF330A23;	// AFE_CTL6, POSTDIV = 2, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x28, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 1.25G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xDF330CA3;	// AFE_CTL6, POSTDIV = 2, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x32, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 1.6G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xBF330823;	// AFE_CTL6, POSTDIV = 1, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x20, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 0
	prn_string("VCO: 9.6G, PLL: 2.4G\n");
	cpior_reg->AFE_CTL[0] = 0x248183FE;
	cpior_reg->AFE_CTL[1] = 0x4527BDF9;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0xD2C18C33;
	cpior_reg->AFE_CTL[7] = 0x00007008;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 2.5G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90081000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xBF330CA3;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 3.2G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0x9F330823;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif

#if 0	// XOVER mode (CK, RDY, DA: cross-over)
	cpior_reg->AFE_CTL[0] |= (0x1F<<10);	// XOVER_CK = 1, XOVER_DA = 1
#endif
#if 0	// Slave: SWAP mode (Swap TXD0 and TXD3; Swap, TXD1 and TXD2; Swap RXD0 and RXD3; Swap RXD1 and RXD2)
	cpior_reg->TEST_C[1] |= (0x7<<9);	// CON_MF = 1, TSW_F = 1, RSW = 1
#endif

	STC_delay_1ms (100);

	AV1_STC_init();	// initial STC1

	// Release PHY RESET
	dly_cnt0 = AV1_GetStc32();
	cpior_reg->PHY_CTRL = 0x80;	// RST_RLS = 1
	while (((cpior_reg->IOP_STS & 0x7F) == 0x0)) {
		//prn_dword(cpior_reg->IOP_STS);
	}
	dly_cnt1 = AV1_GetStc32();

	data_tmp = cpior_reg->IOP_STS;
	prn_string("PHY status change: "); prn_dword(data_tmp);
	if ((data_tmp & 0x7E) != 0x0) {
		prn_string("PHY status check fail!!!\n");
		if ((data_tmp & 0x2) != 0x0)
			prn_string("Low power status changed\n");
		if ((data_tmp & 0x4) != 0x0)
			prn_string("Decode Error!!!\n");
		if ((data_tmp & 0x8) != 0x0)
			prn_string("Connection detection fail!!!\n");
		if ((data_tmp & 0x10) != 0x0)
			prn_string("Lane detection fail!!!\n");
		if ((data_tmp & 0x20) != 0x0)
			prn_string("Mode switch fail!!!\n");
		if ((data_tmp & 0x40) != 0x0)
			prn_string("Location detection timeout!!!\n");
		prn_string("CPIO Initial Finished\n");
	} else {
		prn_string("PHY status check Passed\n");
		prn_string("CPIO Initial Finished\n");
	}

	prn_string("PHY Mode: "); prn_dword(cpior_reg->PHY_CTRL);
	prn_string("Timer start: "); prn_dword(dly_cnt0);
	prn_string("Timer End: "); prn_dword(dly_cnt1);

	//while (1);
}

void cpio_master(void)
{
	int dly_cnt0, dly_cnt1;
	unsigned int data_tmp;

	prn_string("\n\n---- CPIO-R master mode Begin ----\n\n");
	MOON4_REG_AO->sft_cfg[23] = 0x00080000;		// EN_MIPI0_RX = 0

	data_tmp = MOON0_REG_AO->reset[1];
	if ((data_tmp & 0x2) != 0x0) {
		prn_string("CPIO system reset not released, releasing!!\n");
		MOON0_REG_AO->reset[1] = (0x2<<16);	// CPIOR_RESET = 0
		STC_delay_us (100);
	}

	// Change SN
	cpior_reg->IO_SNL = 0x0000A5AA;
	cpior_reg->IO_SNH = 0x80005A5A;

#if 0
	prn_string("VCO: 3.2G, PLL: 0.4G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xFF330823;	// AFE_CTL6, POSTDIV = 3, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x20, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 0.8G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xDF330823;	// AFE_CTL6, POSTDIV = 2, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x20, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 1
	prn_string("VCO: 4.0G, PLL: 1.0G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xDF330A23;	// AFE_CTL6, POSTDIV = 2, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x28, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 1.25G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xDF330CA3;	// AFE_CTL6, POSTDIV = 2, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x32, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 1.6G\n");
	cpior_reg->AFE_CTL[0] = 0x240103FA;	// AFE_CTL0, TXDRVI_CK = 9, SEL_EXT = 1
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;	// AFE_CTL1, RX_DA_RT = 0x12 (R50), RX_CK_RT = 0x12 (R50), TX_DA_RT = 0xF (R100), TX_CK_RT = 0xF (R100), TXDRVIPRE_DA = F, TXDRVI_DA = 9
	cpior_reg->AFE_CTL[2] = 0x00000000;	// AFE_CTL2
	cpior_reg->AFE_CTL[3] = 0x90000000;	// AFE_CTL3, RXCK_EQ1_TC = 4, RXCK_EQ0_TC = 4, EN_RXDA_EQ1 = 0, EN_RXDA_EQ0 = 0
	cpior_reg->AFE_CTL[4] = 0x00000024;	// AFE_CTL4, VREF_SEL = 0 (BG), RXDA_EQ1_TC = 4, RXDA_EQ0_TC = 4
	cpior_reg->AFE_CTL[5] = 0x00004801;	// AFE_CTL5, IRX_ADJ = 4 (25u, 50u), IRX_SEL = 0 (VREF), ITX_ADJ = 4 (50u), ITX_SEL = 0 (VREF), IREF_R_TRIM = 0 (8.03k),  BG_DEC = 0 (400mV), EN_BG = 1
	cpior_reg->AFE_CTL[6] = 0xBF330823;	// AFE_CTL6, POSTDIV = 1, LPFCP = 3, CPFCS = 3, LPFRS = 2, ICH = 3, FREERUN = 1, BNKSEL = 4, FBKDIV = 0x20, PRESCAL = 2, PREDIV = 0, RST_N_TXPLL = 1, EN_TXPLL = 1
	cpior_reg->AFE_CTL[7] = 0x00004000;	// AFE_CTL7, TXPLL_EN_OP = 1
#endif
#if 0
	prn_string("VCO: 9.6G, PLL: 2.4G\n");
	cpior_reg->AFE_CTL[0] = 0x248183FE;
	cpior_reg->AFE_CTL[1] = 0x4527BDF9;
	cpior_reg->AFE_CTL[2] = 0x00000C00;
	cpior_reg->AFE_CTL[3] = 0x48081000;
	cpior_reg->AFE_CTL[4] = 0x000000D2;
	cpior_reg->AFE_CTL[5] = 0x00006801;
	cpior_reg->AFE_CTL[6] = 0xD2C18C33;
	cpior_reg->AFE_CTL[7] = 0x00007008;
#endif
#if 0
	prn_string("VCO: 5.0G, PLL: 2.5G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90081000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0xBF330CA3;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif
#if 0
	prn_string("VCO: 3.2G, PLL: 3.2G\n");
	cpior_reg->AFE_CTL[0] = 0x240183FA;
	cpior_reg->AFE_CTL[1] = 0x2527BCF9;
	cpior_reg->AFE_CTL[2] = 0x00000000;
	cpior_reg->AFE_CTL[3] = 0x90000000;
	cpior_reg->AFE_CTL[4] = 0x000000E4;
	cpior_reg->AFE_CTL[5] = 0x00004801;
	cpior_reg->AFE_CTL[6] = 0x9F330823;
	cpior_reg->AFE_CTL[7] = 0x00005000;
#endif

	STC_delay_1ms (100);

	AV1_STC_init();	// initial STC1

	// Release PHY RESET
	dly_cnt0 = AV1_GetStc32();
	cpior_reg->PHY_CTRL = 0x80;	// RST_RLS = 1
	while (((cpior_reg->IOP_STS & 0x7F) == 0x0)) {
		//prn_dword(cpior_reg->IOP_STS);
	}
	dly_cnt1 = AV1_GetStc32();

	data_tmp = cpior_reg->IOP_STS;
	prn_string("PHY status change: "); prn_dword(data_tmp);
	if ((data_tmp & 0x7E) != 0x0) {
		prn_string("PHY status check fail!!!\n");
		if ((data_tmp & 0x2) != 0x0)
			prn_string("Low power status changed\n");
		if ((data_tmp & 0x4) != 0x0)
			prn_string("Decode Error!!!\n");
		if ((data_tmp & 0x8) != 0x0)
			prn_string("Connection detection fail!!!\n");
		if ((data_tmp & 0x10) != 0x0)
			prn_string("Lane detection fail!!!\n");
		if ((data_tmp & 0x20) != 0x0)
			prn_string("Mode switch fail!!!\n");
		if ((data_tmp & 0x40) != 0x0)
			prn_string("Location detection timeout!!!\n");
		prn_string("CPIO Initial Finished\n");
	} else {
		prn_string("PHY status check Passed\n");
		prn_string("CPIO Initial Finished\n");
	}

	prn_string("PHY Mode: "); prn_dword(cpior_reg->PHY_CTRL);
	prn_string("Timer start: "); prn_dword(dly_cnt0);
	prn_string("Timer End: "); prn_dword(dly_cnt1);

	//while (1);
}
