#ifndef __INC_REGMAP_SP7350_H
#define __INC_REGMAP_SP7350_H

struct moon0_regs_ao {
	unsigned int stamp;            // 0.0
	unsigned int reset[12];        // 0.1 - 0.12
	unsigned int rsvd_1[18];       // 0.13 - 1.30
	unsigned int hw_cfg;           // 0.31

};
#define MOON0_REG_AO ((volatile struct moon0_regs_ao *)RF_GRP_AO(0, 0))

struct moon1_regs_ao {
	unsigned int sft_cfg[32];
};
#define MOON1_REG_AO ((volatile struct moon1_regs_ao *)RF_GRP_AO(1, 0))

struct moon2_regs_ao {
	unsigned int rsvd1;            // 2.0
	unsigned int clken[12];        // 2.1 - 2.12
	unsigned int rsvd2[2];         // 2.13 - 2.14
	unsigned int gclken[12];       // 2.15 - 2.26
	unsigned int rsvd3[5];         // 2.27 - 2.31
};
#define MOON2_REG_AO ((volatile struct moon2_regs_ao *)RF_GRP_AO(2, 0))

struct moon3_regs_ao {
	unsigned int plla_cfg[5];      // 3.0 - 3.4
	unsigned int pllc_cfg[3];      // 3.5 - 3.7
	unsigned int plll3_cfg[3];     // 3.8 - 3.10
	unsigned int plld_cfg[3];      // 3.11 - 3.13
	unsigned int pllh_cfg[3];      // 3.14 - 3.16
	unsigned int plln_cfg[3];      // 3.17 - 3.19
	unsigned int plls_cfg[3];      // 3.20 - 3.22
	unsigned int clkgen[6];        // 3.23 - 3.28
	unsigned int rsvd[3];          // 2.29 - 3.31
};
#define MOON3_REG_AO ((volatile struct moon3_regs_ao *)RF_GRP_AO(3, 0))

struct moon4_regs_ao {
	unsigned int sft_cfg[32];
};
#define MOON4_REG_AO ((volatile struct moon4_regs_ao *)RF_GRP_AO(4, 0))

struct moon5_regs {
	unsigned int sft_cfg[32];
};
#define MOON5_REG ((volatile struct moon5_regs *)RF_GRP_AO(5, 0))

struct gpio_master_regs {
	unsigned int gpio_master[13];
};
#define GPIO_MASTER_REG ((volatile struct gpio_master_regs *)RF_GRP_AO(103, 0)) // 5.0 ~ 5.12

struct gpio_oe_regs {
	unsigned int gpio_oe[13];
};
#define GPIO_OE_REG ((volatile struct gpio_oe_regs *)RF_GRP_AO(103, 13)) // 5.13 ~ 5.25

struct gpio_out_regs {
	unsigned int gpio_out[13];
};
#define GPIO_OUT_REG ((volatile struct gpio_out_regs *)RF_GRP_AO(103, 26)) // 5.26 ~ 6.6

struct pad_ctl_regs {
	unsigned int st_enable[4];		// 101.0 - 101.3
	unsigned int driving_selector0[4];	// 101.4 - 101.7
	unsigned int driving_selector1[4];	// 101.8 - 101.11
	unsigned int driving_selector2[4];	// 101.12 - 101.15
	unsigned int driving_selector3[4];	// 101.16 - 101.19
	unsigned int xtal_config; 		// 101.20
	unsigned int reserved_20;		// 101.21
	unsigned int sd_config;			// 101.22
	unsigned int sdio_config;		// 101.23
	unsigned int reserved_24;		// 101.24
	unsigned int gpio_first[4];		// 101.25 - 101.28
	unsigned int reserved_29[3];		// 101.29 - 101.31
};
#define PAD_CTL_REG ((volatile struct pad_ctl_regs *)RF_GRP_AO(101, 0))

struct pad_ctl2_regs {
	unsigned int sl[2];			// 102.0 - 102.1
	unsigned int gpio_pe[2];		// 102.2 - 102.3
	unsigned int gpio_ps[2];		// 102.4 - 102.5
	unsigned int gpio_spu[2];		// 102.6 - 102.7
	unsigned int dvio_pu[2];		// 102.8 - 102.9
	unsigned int dvio_pd[2]; 		// 102.10 - 102.11
	unsigned int ms;			// 102.12
	unsigned int sdio_pad_power_down;	// 102.13
	unsigned int reserved_14[6];		// 102.14 - 102.19
	unsigned int pnand_softpad_ctrl;	// 102.20
	unsigned int emmc_softpad_ctrl[3];	// 102.21 - 102.23
	unsigned int spi_softpad_ctrl[2];	// 102.24 - 102.25
	unsigned int sd_softpad_ctrl[2];	// 102.26 - 102.27
	unsigned int sdio_softpad_ctrl[2];	// 102.28 - 102.29
	unsigned int gmac_softpad_ctrl[2];	// 102.30 - 102.31
};
#define PAD_CTL2_REG ((volatile struct pad_ctl2_regs *)RF_GRP_AO(102, 0))

struct hb_gp_regs {
	unsigned int hb_otp_data0;
	unsigned int hb_otp_data1;
	unsigned int hb_otp_data2;
	unsigned int hb_otp_data3;
	unsigned int hb_otp_data4;
	unsigned int hb_otp_data5;
	unsigned int hb_otp_data6;
	unsigned int hb_otp_data7;
	unsigned int reserved_8[24];
};
#define HB_GP_REG ((volatile struct hb_gp_regs *)RF_GRP_AO(71, 0))

struct otprx_regs {
	unsigned int sw_trim;          // 351.0
	unsigned int set_key;          // 351.1
	unsigned int reserved_2;       // 351.2
	unsigned int otp_prog_ctl;     // 351.3
	unsigned int otp_prog_addr;    // 351.4
	unsigned int otp_prog_csb;     // 351.5
	unsigned int otp_prog_strobe;  // 351.6
	unsigned int otp_prog_load;    // 351.7
	unsigned int otp_prog_pgenb;   // 351.8
	unsigned int otp_prog_wr;      // 351.9
	unsigned int otp_prog_reg25;   // 351.10
	unsigned int otp_prog_state;   // 351.11
	unsigned int otp_usb_phy_trim; // 351.12
	unsigned int otp_data2;        // 351.13
	unsigned int otp_prog_ps;      // 351.14
	unsigned int otp_rsv2;         // 351.15
	unsigned int key_srst;         // 351.16
	unsigned int otp_ctrl;         // 351.17
	unsigned int otp_cmd;          // 351.18
	unsigned int otp_cmd_status;   // 351.19
	unsigned int otp_addr;         // 351.20
	unsigned int otp_data;         // 351.21
	unsigned int reserved_22[10];  // 351.22
};
#define SP_OTPRX_REG ((volatile struct otprx_regs *)RF_GRP_AO(72, 0))

struct otp_key_regs {
	unsigned int block_addr[4];
	unsigned int reserved_8[28];
};
#define OTP_KEY_REG ((volatile struct otp_key_regs *)RF_GRP_AO(73, 0))

struct uart_regs {
	unsigned int dr;  /* data register */
	unsigned int lsr; /* line status register */
	unsigned int msr; /* modem status register */
	unsigned int lcr; /* line control register */
	unsigned int mcr; /* modem control register */
	unsigned int div_l;
	unsigned int div_h;
	unsigned int isc; /* interrupt status/control */
	unsigned int txr; /* tx residue */
	unsigned int rxr; /* rx residue */
	unsigned int thr; /* rx threshold */
	unsigned int clk_src;
	unsigned int smp_rate;
};
#define UART0_REG    ((volatile struct uart_regs *)RF_GRP_AO(50, 0))
#define UART1_REG    ((volatile struct uart_regs *)RF_GRP_AO(51, 0))
#define UART2_REG    ((volatile struct uart_regs *)RF_GRP_AO(55, 0))
#define UART3_REG    ((volatile struct uart_regs *)RF_GRP_AO(59, 0))
#define UART6_REG    ((volatile struct uart_regs *)RF_GRP_AO(63, 0))
#define UART7_REG    ((volatile struct uart_regs *)RF_GRP_AO(67, 0))

#define UADBG_REG    ((volatile struct uart_regs *)RF_GRP(498, 0))

struct ua2axi_regs {
	unsigned int timeout_cycle;
	unsigned int timeout_remain_cnt;
	unsigned int debug_mode;
	unsigned int axi_en;
	unsigned int encode_base;
};
#define UA2AXI_REG    ((volatile struct ua2axi_regs *)RF_GRP(499, 0))

struct stc_regs {
	unsigned int stc_31_0;       // 12.0
	unsigned int stc_63_32;      // 12.1
	unsigned int stc_64;         // 12.2
	unsigned int stc_divisor;    // 12.3
	unsigned int stc_config;     // 12.4
	unsigned int rtc_23_0;       // 12.5
	unsigned int rtc_divisor;    // 12.6
	unsigned int timerw_ctl;     // 12.7
	unsigned int timerw_cnt;     // 12.8
	unsigned int timer0_ctl;     // 12.9
	unsigned int timer0_cnt;     // 12.10
	unsigned int timer0_reload;  // 12.11
	unsigned int timer1_ctl;     // 12.12
	unsigned int timer1_cnt;     // 12.13
	unsigned int timer1_reload;  // 12.14
	unsigned int timer2_ctl;     // 12.15
	unsigned int timer2_cnt;     // 12.16
	unsigned int timer2_reload;  // 12.17
	unsigned int timer3_ctl;     // 12.18
	unsigned int timer3_cnt31_0; // 12.19
	unsigned int timer3_cnt63_32;// 12.20
	unsigned int timer3_reload31_0;// 12.21
	unsigned int timer3_reload63_32;// 12.22
	unsigned int stcl_31_0;       // 12.23
	unsigned int stcl_32;         // 12.24

};

#define STC_REG     ((volatile struct stc_regs *)RF_GRP_AO(23, 0))
#define STC_AV0_REG ((volatile struct stc_regs *)RF_GRP_AO(24, 0))
#define STC_AV1_REG ((volatile struct stc_regs *)RF_GRP_AO(25, 0))
#define STC_AV2_REG ((volatile struct stc_regs *)RF_GRP_AO(26, 0))


struct dpll_regs {
	unsigned int dpll0_ctrl;
	unsigned int dpll0_remainder;
	unsigned int dpll0_denominator;
	unsigned int dpll0_divider;
	unsigned int g20_reserved_0[4];
	unsigned int dpll1_ctrl;
	unsigned int dpll1_remainder;
	unsigned int dpll1_denominator;
	unsigned int dpll1_divider;
	unsigned int g20_reserved_1[4];
	unsigned int dpll2_ctrl;
	unsigned int dpll2_remainder;
	unsigned int dpll2_denominator;
	unsigned int dpll2_divider;
	unsigned int g20_reserved_2[4];
	unsigned int dpll3_ctrl;
	unsigned int dpll3_remainder;
	unsigned int dpll3_denominator;
	unsigned int dpll3_divider;
	unsigned int dpll3_sprd_num;
	unsigned int g20_reserved_3[3];
};
#define DPLL_REG     ((volatile struct dpll_regs *)RF_GRP(20, 0))

struct spi_ctrl_regs {
	unsigned int spi_ctrl;       // 22.0
	unsigned int spi_timing;     // 22.1
	unsigned int spi_page_addr;  // 22.2
	unsigned int spi_data;       // 22.3
	unsigned int spi_status;     // 22.4
	unsigned int spi_auto_cfg;   // 22.5
	unsigned int spi_cfg[3];     // 22.6
	unsigned int spi_data_64;    // 22.9
	unsigned int spi_buf_addr;   // 22.10
	unsigned int spi_statu_2;    // 22.11
	unsigned int spi_err_status; // 22.12
	unsigned int spi_data_addr;  // 22.13
	unsigned int mem_parity_addr;// 22.14
	unsigned int spi_col_addr;   // 22.15
	unsigned int spi_bch;        // 22.16
	unsigned int spi_intr_msk;   // 22.17
	unsigned int spi_intr_sts;   // 22.18
	unsigned int spi_page_size;  // 22.19
	unsigned int g20_reserved_0; // 22.20
};
#define SPI_CTRL_REG ((volatile struct spi_ctrl_regs *)RF_GRP(22, 0))

/* start of xhci */
struct uphy_u3_regs {
	unsigned int cfg[32];		       // 189.0
};

#define UPHY0_U3_REG ((volatile struct uphy_u3_regs *)RF_AMBA(189, 0))
#define UPHY1_U3_REG ((volatile struct uphy_u3_regs *)RF_AMBA(190, 0))
#define XHCI0_REG ((volatile struct xhci_hccr *)RF_AMBA(161, 0))
#define XHCI1_REG ((volatile struct xhci_hccr *)RF_AMBA(175, 0))
/* end of xhci */

struct uphy_rn_regs {
	u32 cfg[28];			//  0 - 27
	u32 gctrl[3];			// 28 - 30
	u32 gsts;			// 31
};
#define UPHY0_RN_REG ((volatile struct uphy_rn_regs *)RF_GRP(149, 0))
#define UPHY1_RN_REG ((volatile struct uphy_rn_regs *)RF_GRP(150, 0))

/* usb host */
struct ehci_regs {
	unsigned int ehci_len_rev;
	unsigned int ehci_sparams;
	unsigned int ehci_cparams;
	unsigned int ehci_portroute;
	unsigned int g143_reserved_0[4];
	unsigned int ehci_usbcmd;
	unsigned int ehci_usbsts;
	unsigned int ehci_usbintr;
	unsigned int ehci_frameidx;
	unsigned int ehci_ctrl_ds_segment;
	unsigned int ehci_prd_listbase;
	unsigned int ehci_async_listaddr;
	unsigned int g143_reserved_1[9];
	unsigned int ehci_config;
	unsigned int ehci_portsc;
	/*
	unsigned int g143_reserved_2[1];
	unsigned int ehci_version_ctrl;
	unsigned int ehci_general_ctrl;
	unsigned int ehci_usb_debug;
	unsigned int ehci_sys_debug;
	unsigned int ehci_sleep_cnt;
	*/
};
#define EHCI0_REG ((volatile struct ehci_regs *)AHB_GRP(2, 2, 0)) // 0xf8102100
#define EHCI1_REG ((volatile struct ehci_regs *)AHB_GRP(3, 2, 0)) // 0xf8103100

struct usbh_sys_regs {
	unsigned int uhversion;
	unsigned int reserved[3];
	unsigned int uhpowercs_port;
	unsigned int uhc_fsm_axi;
	unsigned int reserved2[22];
	unsigned int uho_fsm_st1;
	unsigned int uho_fsm_st2;
	unsigned int uhe_fsm_st1;
	unsigned int uhe_fsm_st2;
};
#define USBH0_SYS_REG ((volatile struct usbh_sys_regs *)AHB_GRP(2, 0, 0)) // 0xf8102000
#define USBH1_SYS_REG ((volatile struct usbh_sys_regs *)AHB_GRP(3, 0, 0)) // 0xf8103000

struct emmc_ctl_regs {
	/*g0.0*/
	unsigned int mediatype:3;
	unsigned int reserved0:1;
	unsigned int dmasrc:3;
	unsigned int reserved1:1;
	unsigned int dmadst:3;
	unsigned int reserved2:21;

	/*g0.1*/
	unsigned int card_ctl_page_cnt:16;
	unsigned int reserved3:16;

	/* g0.2 */
	unsigned int sdram_sector_0_size:16;
	unsigned int reserved4:1;
	/* g0.3 */
	unsigned int dma_base_addr;
	/* g0.4 */
	union {
		struct {
			unsigned int reserved5:1;
			unsigned int hw_dma_en:1;
			unsigned int reserved6:1;
			unsigned int hw_sd_hcsd_en:1;
			unsigned int hw_sd_dma_type:2;
			unsigned int hw_sd_cmd13_en:1;
			unsigned int reserved7:1;
			unsigned int stop_dma_flag:1;
			unsigned int hw_dma_rst:1;
			unsigned int dmaidle:1;
			unsigned int dmastart:1;
			unsigned int hw_block_num:2;
			unsigned int reserved8:2;
			unsigned int hw_cmd13_rca:16;
		};
		unsigned int hw_dma_ctl;
	};
	/* g0.5 */
	union {
		struct {
			unsigned int reg_sd_ctl_free:1;				// 0
			unsigned int reg_sd_free:1;					// 1
			unsigned int reg_ms_ctl_free:1;				// 2
			unsigned int reg_ms_free:1;					// 3
			unsigned int reg_dma_fifo_free:1;			// 4
			unsigned int reg_dma_ctl_free:1;			// 5
			unsigned int reg_hwdma_page_free:1;			// 6
			unsigned int reg_hw_dma_free:1;				// 7
			unsigned int reg_sd_hwdma_free:1;			// 8
			unsigned int reg_ms_hwdma_free:1;			// 9
			unsigned int reg_dma_reg_free:1;			// 10
			unsigned int reg_card_reg_free:1;			// 11
			unsigned int reserved9:20;
		};
		unsigned int card_gclk_disable;
	};

	/* g0.6 ~ g0.19*/
	struct {
		unsigned int dram_sector_addr;
		unsigned int sdram_sector_size:16;
		unsigned int reserved10:16;
	} dma_addr_info[7];

	/* g0.20 */
	union {
		struct {
			unsigned int dram_sector_cnt:3;				// 2:00 ro
			unsigned int hw_block_cnt:2;				// 04:03 ro
			unsigned int reserved11:11;					// 15:05 ro
			unsigned int hw_page_cnt:16;				// 31:16 ro
		};
		unsigned int sdram_sector_block_cnt;
	};
	/* g0.20 ~ g0.28 */
	unsigned int dma_hw_page_addr[4];
	unsigned int dma_hw_page_num[4];

	/* g0.29 */
	unsigned int hw_wait_num;

	/* g0.30 */
	unsigned int hw_delay_num:16;
	unsigned int reserved12:16;

	/* g0.31 */
	union {
		struct {
			unsigned int incnt:11;
			unsigned int outcnt:11;
			unsigned int dma_sm:3;
			unsigned int reserved13:7;
		};
		unsigned int dma_debug;
	};

	/* g1.0 */
	union {
		struct {
			unsigned int boot_ack_en:1;
			unsigned int boot_ack_tmr:1;
			unsigned int boot_data_tmr:1;
			unsigned int fast_boot:1;
			unsigned int boot_mode:1;
			unsigned int bootack:3;
			unsigned int reserved14:24;
		};
		unsigned int boot_ctl;
	};

	/* g1.1 */
	union {
		struct {
			unsigned int vol_tmr:2;
			unsigned int sw_set_vol:1;
			unsigned int hw_set_vol:1;
			unsigned int vol_result:2;
			unsigned int reserved15:26;
		};
		unsigned int sd_vol_ctrl;
	};
	/* g1.2 */
	union {
		struct {
			unsigned int sdcmpen:1;
			unsigned int sd_cmp:1;   //1
			unsigned int sd_cmp_clr:1;   //2
			unsigned int sdio_int_en:1;  //3
			unsigned int sdio_int:1; //4
			unsigned int sdio_int_clr:1; //5
			unsigned int detect_int_en:1;    //6
			unsigned int detect_int:1;   //7
			unsigned int detect_int_clr:1;   //8
			unsigned int hwdmacmpen:1;   //9
			unsigned int hw_dma_cmp:1;   //10
			unsigned int hwdmacmpclr:1;  //11
			unsigned int reserved16:20; //31:12
		};
		unsigned int sd_int;
	};

	/* g1.3 */
	unsigned int sd_page_num:16;
	unsigned int reserved17:16;
	/* g1.4 */
	union {
		struct {
			unsigned int sdpiomode:1;
			unsigned int sdddrmode:1;
			unsigned int sd_len_mode:1;
			unsigned int first_dat_hcyc:1;
			unsigned int sd_trans_mode:2;
			unsigned int sdautorsp:1;
			unsigned int sdcmddummy:1;
			unsigned int sdrspchk_en:1;
			unsigned int sdiomode:1;
			unsigned int sdmmcmode:1;
			unsigned int sddatawd:1;
			unsigned int sdrsptmren:1;
			unsigned int sdcrctmren:1;
			unsigned int rx4_en:1;
			unsigned int sdrsptype:1;
			unsigned int detect_tmr:2;
			unsigned int mmc8_en:1;
			unsigned int selci:1;
			unsigned int sdfqsel:12;
		};
		unsigned int sd_config0;
	};

	/* g1.5 */
	union {
		struct {
			unsigned int rwc:1;
			unsigned int s4mi:1;
			unsigned int resu:1;
			unsigned int sus_req:1;
			unsigned int con_req:1;
			unsigned int sus_data_flag:1;
			unsigned int int_multi_trig:1;
			unsigned int reserved18:25;
		};
		unsigned int sdio_ctrl;
	};

	/* g1.6 */
	union {
		struct {
			unsigned int sdrst:1;
			unsigned int sdcrcrst:1;
			unsigned int sdiorst:1;
			unsigned int reserved19:29;
		};
		unsigned int sd_rst;
	};

	/* g1.7 */
	union {
		struct {
			unsigned int sdctrl0:1;
			unsigned int sdctrl1:1;
			unsigned int sdioctrl:1;
			unsigned int emmcctrl:1;
			unsigned int reserved20:28;
		} ;
		unsigned int sd_ctrl;
	};
	/* g1.8 */
	union {
		struct {
			unsigned int sdstatus:19;
			unsigned int reserved21:13;
		};
		unsigned int sd_status;
	};
	/* g1.9 */
	union {
		struct {
			unsigned int sdstate:3;
			unsigned int reserved22:1;
			unsigned int sdcrdcrc:3;
			unsigned int reserved23:1;
			unsigned int sdstate_new:7;
			unsigned int reserved24:17;
		};
		unsigned int sd_state;
	};

	/* g1.10 */
	union {
		struct {
			unsigned int hwsd_sm:10;
			unsigned int reserved25:22;
		};
		unsigned int sd_hw_state;
	};

	/* g1.11 */
	union {
		struct {
			unsigned int sddatalen:11;
			unsigned int reserved26:21;
		};
		unsigned int sd_blocksize;
	};

	/* g1.12 */
	union {
		struct {
			unsigned int tx_dummy_num:9;
			unsigned int sdcrctmr:11;
			unsigned int sdrsptmr:11;
			unsigned int sd_high_speed_en:1;
		};
		unsigned int sd_config1;
	};

	/* g1.13 */
	union {
		struct {
			unsigned int sd_clk_dly_sel:3;
			unsigned int reserved27:1;
			unsigned int sd_wr_dat_dly_sel:3;
			unsigned int reserved28:1;
			unsigned int sd_wr_cmd_dly_sel:3;
			unsigned int reserved28_1:1;
			unsigned int sd_rd_rsp_dly_sel:3;
			unsigned int reserved28_2:1;
			unsigned int sd_rd_dat_dly_sel:3;
			unsigned int reserved28_3:1;
			unsigned int sd_rd_crc_dly_sel:3;
			unsigned int reserved29:9;
		};
		unsigned int sd_timing_config;
	};

	/* g1.14 */
	unsigned int sd_rxdattmr:29;
	unsigned int reserved30:3;

	/* g1.15 */
	unsigned int sd_piodatatx:16;
	unsigned int reserved31:16;

	/* g1.16 */
	unsigned int sd_piodatarx;

	/* g1.17 */
	/* g1.18 */
	unsigned char sd_cmdbuf[5];
	unsigned char reserved32[3];
	/* g1.19 - g1.20 */
	unsigned int sd_rspbuf0_3;
	unsigned int sd_rspbuf4_5;
	/*  unused sd control regs */
	unsigned int reserved34[11];
	/* ms card related regs */
	unsigned int ms_regs[32];
};
#define CARD0_CTL_REG ((volatile int  *)RF_GRP(118, 0))
#define CARD1_CTL_REG ((volatile int  *)RF_GRP(125, 0))

struct dw_i2c_regs {
	unsigned int ic_con;		/* 00 */
	unsigned int ic_tar;		/* 01 */
	unsigned int ic_sar;		/* 02 */
	unsigned int hs_maddr;		/* 03 */
	unsigned int ic_data_cmd;	/* 04 */
	unsigned int ic_ss_scl_hcnt;	/* 05 */
	unsigned int ic_ss_scl_lcnt;	/* 06 */
	unsigned int ic_fs_scl_hcnt;	/* 07 */
	unsigned int ic_fs_scl_lcnt;	/* 08 */
	unsigned int ic_hs_scl_hcnt;	/* 09 */
	unsigned int ic_hs_scl_lcnt;	/* 10 */
	unsigned int ic_intr_stat;	/* 11 */
	unsigned int ic_intr_mask;	/* 12 0x30*/
	unsigned int ic_raw_intr_stat;	/* 13 */
	unsigned int ic_rx_tl;		/* 14 */
	unsigned int ic_tx_tl;		/* 15 */
	unsigned int ic_clr_intr;	/* 16 0x40*/
	unsigned int ic_clr_rx_under;	/* 17 */
	unsigned int ic_clr_rx_over;	/* 18 */
	unsigned int ic_clr_tx_over;	/* 19 */
	unsigned int ic_clr_rd_req;	/* 20 0x50*/
	unsigned int ic_clr_tx_abrt;	/* 21 */
	unsigned int ic_clr_rx_done;	/* 22 */
	unsigned int ic_clr_tx_activity;	/* 23 */
	unsigned int ic_clr_stop_det;	/* 24 0x60*/
	unsigned int ic_clr_start_dft;	/* 25 */
	unsigned int ic_clr_gen_call;	/* 26 */
	unsigned int ic_enable;		/* 27 */
	unsigned int ic_status;		/* 28 0x70*/
	unsigned int ic_txflr;		/* 29 */
	unsigned int ic_rxflr;		/* 30 */
	unsigned int ic_sda_hold;	/* 31*/
	unsigned int ic_tx_abrt_source;	/* 32 0x80*/
	unsigned int g40_reserved_0[6];	/* 38 */
	unsigned int ic_enable_status;	/* 39 0x9c*/
	unsigned int g40_reserved_1[2];	/* 40 */
	unsigned int ic_clr_restart_det;	/* 42 0xa8*/
	unsigned int g40_reserved_2[18];	/* 43 */
	unsigned int ic_comp_param_1;	/* 61 0xf4*/
	unsigned int ic_comp_version;	/* 62 0xf8*/
	unsigned int ic_comp_type;	/* 63 0xfc*/

};
#define I2C0_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(40, 0))
#define I2C1_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(41, 0))
#define I2C2_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(42, 0))
#define I2C3_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(43, 0))
#define I2C4_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(44, 0))
#define I2C5_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(45, 0))
#define I2C6_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(46, 0))
#define I2C7_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(47, 0))
#define I2C8_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(48, 0))
#define I2C9_REG_AO ((volatile struct dw_i2c_regs *)RF_AMBA_AO(49, 0))

struct dw_adc_regs {
	unsigned int adc_cfg00;			/* 00 */
	unsigned int adc_cfg01;			/* 01 */
	unsigned int adc_cfg02;			/* 02 */
	unsigned int adc_cfg03;			/* 03 */
	unsigned int adc_cfg04;			/* 04 */
	unsigned int adc_cfg05;			/* 05 */
	unsigned int adc_cfg06;			/* 06 */
	unsigned int adc_cfg07;			/* 07 */
	unsigned int adc_cfg08;			/* 08 */
	unsigned int adc_cfg09;			/* 09 */
	unsigned int adc_cfg0a;			/* 10 */
	unsigned int adc_cfg0b;			/* 11 */
	unsigned int adc_cfg0c;			/* 12 0x30*/
	unsigned int adc_cfg0d;			/* 13 */
	unsigned int adc_cfg0e;			/* 14 */
	unsigned int adc_cfg0f;			/* 15 */
	unsigned int adc_cfg10;			/* 16 0x40*/
	unsigned int adc_cfg11;			/* 17 */
	unsigned int adc_cfg12;			/* 18 */
	unsigned int adc_cfg13;			/* 19 */
	unsigned int adc_cfg14;			/* 20 0x50*/
	unsigned int adc_reserved[20];		/*43 */

};
#define ADC_REG_AO ((volatile struct dw_adc_regs *)RF_GRP_AO(94, 0))

/* sd card regs */
struct card_ctl_regs {
	unsigned int card_mediatype;     // 0
	unsigned int card_cpu_page_cnt;      // 1
	unsigned int sdram_sector0_sz;       // 2
	unsigned int dma_base_addr;          // 3
	unsigned int hw_dma_ctl;             // 4
	unsigned int card_gclk_disable;      // 5
	unsigned int sdram_sector1_addr;     // 6
	unsigned int sdram_sector1_sz;       // 7
	unsigned int sdram_sector2_addr;     // 8
	unsigned int sdram_sector2_sz;       // 9
	unsigned int sdram_sector3_addr;     // 10
	unsigned int sdram_sector3_sz;       // 11
	unsigned int sdram_sector4_addr;     // 12
	unsigned int sdram_sector4_sz;       // 13
	unsigned int sdram_sector5_addr;     // 14
	unsigned int sdram_sector5_sz;       // 15
	unsigned int sdram_sector6_addr;     // 16
	unsigned int sdram_sector6_sz;       // 17
	unsigned int sdram_sector7_addr;     // 18
	unsigned int sdram_sector7_sz;       // 19
	unsigned int sdram_sector_block_cnt; // 20
  unsigned int dma_hw_page_addr0;      // 21
  unsigned int dma_hw_page_addr1;      // 22
  unsigned int dma_hw_page_addr2;      // 23
  unsigned int dma_hw_page_addr3;      // 24
  unsigned int dma_hw_page_num0;       // 25
  unsigned int dma_hw_page_num1;       // 26
  unsigned int dma_hw_page_num2;       // 27
  unsigned int dma_hw_page_num3;       // 28
	unsigned int dma_hw_wait_num;        // 29
	unsigned int dma_hw_delay_num;       // 30
	unsigned int dma_debug;              // 31
};

struct card_sd_regs {
	unsigned int boot_ctl;               // 0
	unsigned int sd_vol_ctrl;            // 1
	unsigned int sd_int;                 // 2
	unsigned int sd_page_num;            // 3
	unsigned int sd_config0;             // 4
	unsigned int sdio_ctrl;              // 5
	unsigned int sd_rst;                 // 6
	unsigned int sd_ctrl;                // 7
	unsigned int sd_status;              // 8
	unsigned int sd_state;               // 9
	unsigned int sd_hw_state;            // 10
	unsigned int sd_blocksize;           // 11
	unsigned int sd_config1;             // 12
	unsigned int sd_timing_config0;      // 13
	unsigned int sd_rx_data_tmr;         // 14
	unsigned int sd_piodatatx;           // 15
	unsigned int sd_piodatarx;           // 16
	unsigned char sd_cmdbuf[5];          // 17
	unsigned char reserved_char01[3];    // 18
	unsigned int sd_rspbuf0_3;           // 19
	unsigned int sd_rspbuf4_5;           // 20
	unsigned int reserved35[11];         // 21
};
#define CARD0_SD_REG ((volatile struct card_sd_regs *)RF_GRP(119, 0))

struct card_sdms_regs {
	unsigned int sd_rspbuf0_3;       // 0
	unsigned int sd_rspbuf4_5;       // 1
	unsigned int sd_crc16even[4];    // 2
	unsigned int sd_crc7buf;         // 6
	unsigned int sd_crc16buf0;       // 7
	unsigned int sd_hw_state;        // 8
	unsigned int sd_crc16buf1;       // 9
	unsigned int sd_hw_cmd13_rca;    // 10
	unsigned int sd_crc16buf2;       // 11
	unsigned int sd_tx_dummy_num;    // 12
	unsigned int sd_crc16buf3;       // 13
	unsigned int sd_clk_dly;         // 14
	unsigned int reserved15;         // 15
	unsigned int ms_piodmarst;       // 16
	unsigned int ms_cmd;             // 17
	unsigned int reserved18;         // 18
	unsigned int ms_hw_state;        // 19
	unsigned int ms_modespeed;       // 20
	unsigned int ms_timeout;         // 21
	unsigned int ms_state;           // 22
	unsigned int ms_status;          // 23
	unsigned int ms_rddata[4];       // 24
	unsigned int ms_crcbuf[2];       // 28
	unsigned int ms_crc_error;       // 30
	unsigned int ms_piordy;          // 31
};
#define CARD0_SDMS_REG ((volatile struct card_sdms_regs *)RF_GRP(120, 0))

struct card_ms_regs {
	unsigned int ms_wd_data[16];     // 0
	unsigned int reserved16[16];     // 16
};
#define CARD0_MS_REG ((volatile struct card_ms_regs *)RF_GRP(121, 0))

struct card_dma_regs {
	unsigned int dma_data;             // 0
	unsigned int dma_srcdst;           // 1
	unsigned int dma_size;             // 2
	unsigned int dma_hw_stop_rst;      // 3
	unsigned int dma_ctrl;             // 4
	unsigned int dma_base_addr[2];     // 5
	unsigned int dma_hw_en;            // 7
	unsigned int dma_hw_page_addr0[2]; // 8
	unsigned int dma_hw_page_addr1[2]; // 10
	unsigned int dma_hw_page_addr2[2]; // 12
	unsigned int dma_hw_page_addr3[2]; // 14
	unsigned int dma_hw_page_num[4];   // 16
	unsigned int dma_hw_block_num;     // 20
	unsigned int dma_start;            // 21
	unsigned int dma_hw_page_cnt;      // 22
	unsigned int dma_cmp;              // 23
	unsigned int dma_int_en;           // 24
	unsigned int reserved25;           // 25
	unsigned int dma_hw_wait_num[2];   // 26
	unsigned int dma_hw_delay_num;     // 28
	unsigned int reserved29[3];        // 29
};

/* NAND */
#define NAND_S330_BASE           0xF8002B80     // RG_GRP(87, 0)
#define BCH_S338_BASE_ADDRESS    0xF8101000     // RG_AMBA(257, 0)

/* SPI NAND */
#define CONFIG_SP_SPINAND_BASE   ((volatile unsigned int *)RF_GRP(87, 0))
#define SPI_NAND_DIRECT_MAP      0xF4000000


struct rgst_secure_regs {
	unsigned int cfg[32];              // 502.0~31
};
#define RGST_SECURE_REG ((volatile struct rgst_secure_regs *)RF_GRP(502, 0))

struct amba_secure_regs {
	unsigned int cfg[32];              // 502.0~31
};
#define AMBA_SECURE_REG ((volatile struct amba_secure_regs *)RF_GRP(503, 0))

/* TSGEN Write/Read */
#define ARM_TSGEN_WR_BASE  0xf810a000

struct gpio_first_regs {
	unsigned int gpio_first[7];
};
#define GPIO_FIRST_REG ((volatile struct gpio_first_regs *)RF_GRP_AO(101, 25)) // 101.25 ~ 101.31

struct gpio_in_regs {
	unsigned int gpio_in[7];
};
#define GPIO_IN_REG ((volatile struct gpio_in_regs *)RF_GRP_AO(104, 7)) // 104.7 ~ 104.13

struct pad_dvio_regs {
	unsigned int gmx_21_27:1;    /* spi-nor spi_nand X2 p-nand x1*/
	unsigned int gmx_28_37:1;    /* emmc spi_nand X1 p-nand x1*/
	unsigned int ao_mx_0_9:1;    /* uart 0/1/2 */
	unsigned int ao_mx_10_19:1;  /* pwm2/3 uart3 spi_cb0 i2c0 */
	unsigned int ao_mx_20_29:1;  /* i2c1 spi_cb4 i2c2 pwm0/1  */
	unsigned int reserved2:27;
};
#define PAD_DVIO_REG ((volatile struct pad_dvio_regs *)RF_GRP_AO(102, 12))

struct rgst_cpio {
	unsigned int CH0_ADDR;
	unsigned int CH0_CTRL;
	unsigned int CH1_ADDR;
	unsigned int CH1_CTRL;
	unsigned int DMA_RSVD[8];
	unsigned int IO_CTRL;
	unsigned int IO_STS;
	unsigned int SRC_ADR;
	unsigned int DST_ADR;
	unsigned int IO_TCTL;
	unsigned int RA_AMSK;
	unsigned int ARB_CTL;
	unsigned int IO_RSVD0;
	unsigned int INT_IT[4];
	unsigned int INT_OT[4];
	unsigned int IO_RSVD1[35];
	unsigned int I2C_CTRL;
	unsigned int IO_RSVD2[64];
	unsigned int TEST_D[6];
	unsigned int TEST_C[2];
	unsigned int IOP_STS;
	unsigned int PHY_CTRL;
	unsigned int IO_SNL;
	unsigned int IO_SNH;
	unsigned int AFE_CTL[8];
	unsigned int AFE_STS[2];
	unsigned int IO_RSVD3[34];
	unsigned int IO_DBG[8];
};
#define cpior_reg ((volatile struct rgst_cpio *)0xf8106000)

struct pmc_regs_ao {
        unsigned int pmc_ctrl;
        unsigned int pmc_timer;
        unsigned int pmc_timer2;
        unsigned int pmc_maindomain_pwd_h;
        unsigned int pmc_maindomain_pwd_l;
        unsigned int pmc_xtal32k_pwd_h;
        unsigned int pmc_xtal32k_pwd_l;
        unsigned int pmc_xtal27m_pwd_h;
        unsigned int pmc_xtal27m_pwd_l;
        unsigned int pmc_wakeup_latch;
        unsigned int pmc_ivmx_reg;
        unsigned int pmc_reserve11;
        unsigned int pmc_reserve12;
        unsigned int pmc_reserve13;
        unsigned int pmc_reserve14;
        unsigned int pmc_main_pwr_ctrl;
        unsigned int pmc_corepsw_pwd ;
        unsigned int pmc_corepsw_en;
        unsigned int pmc_coremem_sdpwd;
        unsigned int pmc_coremem_sden;
        unsigned int pmc_lvs_pwd;
        unsigned int pmc_lvs_disable;
        unsigned int pmc_iso_pwd;
        unsigned int pmc_iso_en;
        unsigned int pmc_reserve24;
        unsigned int pmc_reserve25;
        unsigned int pmc_reserve26;
        unsigned int pmc_reset_vector;
        unsigned int pmc_ca55_reset_state;
        unsigned int pmc_pctl_reg;
        unsigned int pmc_pctl_int_clr;
        unsigned int pmc_reserve31;
};
#define PMC_REGS ((volatile struct pmc_regs_ao *) RF_GRP_AO(36, 0))

#endif /* __INC_REGMAP_SP7350_H */
