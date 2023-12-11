sinclude .config

###########  ARCH CPU_PATH config ######################
ifeq ($(CONFIG_ARCH_ARM), y)
ARCH := arm
else ifeq ($(CONFIG_ARCH_RISCV), y)
ARCH := riscv
endif

CPU_PATH = arm/sp7350

###########  Toolchain path ######################
ifneq ($(CROSS),)
CC = $(CROSS)gcc
CPP = $(CROSS)cpp
OBJCOPY = $(CROSS)objcopy
OBJDUMP = $(CROSS)objdump
endif

###########  LDFLAGS CONFIG ######################
LD_GEN   = arch/$(CPU_PATH)/boot.ld
LD_SRC   = arch/$(CPU_PATH)/boot.ldi
LDFLAGS  = -L $(shell dirname `$(CC) -print-libgcc-file-name`) -lgcc
#LDFLAGS += -Wl,--gc-sections,--print-gc-sections
LDFLAGS += -Wl,--gc-sections
LDFLAGS +=  -Wl,--build-id=none

###########  CFLAGS CONFIG ######################
CFLAGS   = -Os -Wall -g -nostdlib -fno-builtin -Iinclude -Iarch/$(CPU_PATH)/include -I.
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -fno-pie -fno-PIE -fno-pic -fno-PIC
CFLAGS += -fno-partial-inlining -fno-jump-tables
CFLAGS += -static

ifeq ($(ARCH),arm)
CFLAGS  += -mthumb -mthumb-interwork
else
CFLAGS	+= -march=rv64gc -mabi=lp64d -mcmodel=medany -msave-restore
endif

CFLAGS  += -Iinclude/arm
CFLAGS  += -march=armv8-a -fno-delete-null-pointer-checks
CFLAGS  += -mno-unaligned-access
CFLAGS  += -ffunction-sections -fdata-sections
CFLAGS  += -Wno-unused-function
ifeq ($(SECURE),1)
CFLAGS  += -DCONFIG_COMPILE_WITH_SECURE=1
CFLAGS  += -DCONFIG_SECURE_BOOT_SIGN=1
else
CFLAGS  += -DCONFIG_COMPILE_WITH_SECURE=0
endif
ifeq ($(ENCRYPTION),1)
CFLAGS  += -DCONFIG_ENCRYPTION
endif

################# xboot size config ################
XBOOT_MAX =$$((96 * 1024))

#################### make #######################
.PHONY: release debug

# default target
release debug: all

BIN     := bin
TARGET  := xboot
MAKE 	= make

all: chkconfig auto_config
	@$(MAKE) $(TARGET)

	@# 32-byte xboot header
ifeq ($(CONFIG_LPDDR4),y)
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/lpddr4/lpddr4_pmu_train_imem.bin $(BIN)/pmu_train_imem.img 0 im1d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/lpddr4/lpddr4_pmu_train_dmem.bin $(BIN)/pmu_train_dmem.img 0 dm1d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/lpddr4/lpddr4_2d_pmu_train_imem.bin $(BIN)/2d_pmu_train_imem.img 0 im2d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/lpddr4/lpddr4_2d_pmu_train_dmem.bin $(BIN)/2d_pmu_train_dmem.img 0 dm2d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/lpddr4/lpddr4_diags_imem.bin $(BIN)/diags_imem.img 0 imda
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/lpddr4/lpddr4_diags_dmem.bin $(BIN)/diags_dmem.img 0 dmda
endif
ifeq ($(CONFIG_DDR4),y)
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/ddr4/ddr4_pmu_train_imem.bin $(BIN)/pmu_train_imem.img 0 im1d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/ddr4/ddr4_pmu_train_dmem.bin $(BIN)/pmu_train_dmem.img 0 dm1d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/ddr4/ddr4_2d_pmu_train_imem.bin $(BIN)/2d_pmu_train_imem.img 0 im2d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/ddr4/ddr4_2d_pmu_train_dmem.bin $(BIN)/2d_pmu_train_dmem.img 0 dm2d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/ddr4/ddr4_diags_imem.bin $(BIN)/diags_imem.img 0 imda
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/ddr4/ddr4_diags_dmem.bin $(BIN)/diags_dmem.img 0 dmda
endif
ifeq ($(CONFIG_DDR3),y)
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/ddr3/ddr3_pmu_train_imem.bin $(BIN)/pmu_train_imem.img 0 im1d
	@bash ./add_xhdr.sh ../draminit/dwc/firmware/SP7350/ddr3/ddr3_pmu_train_dmem.bin $(BIN)/pmu_train_dmem.img 0 dm1d
endif

ifeq ($(CONFIG_STANDALONE_DRAMINIT), y)
	@# print draminit.img size
	@sz=`du -sb $(DRAMINIT_IMG) | cut -f1` ; \
	    printf "$(DRAMINIT_IMG) size = %d (hex %x)\n" $$sz $$sz
	@echo "Append $(DRAMINIT_IMG)"
	@# xboot.img = xboot.img.orig + draminit.img
	@mv  $(BIN)/$(TARGET).img  $(BIN)/$(TARGET).img.orig
	@cat $(BIN)/$(TARGET).img.orig $(DRAMINIT_IMG) > $(BIN)/$(TARGET).img
else
	@echo "Linked with $(DRAMINIT_OBJ)"
endif

size_check:
	@# print xboot size
	@sz=`du -sb bin/$(TARGET).img | cut -f1` ; \
	 printf "xboot.img size = %d (hex %x)\n" $$sz $$sz ; \
	 if [ $$sz -gt $(XBOOT_MAX) ];then \
		echo -e "\033[0;1;31;40mxboot size limit is $(XBOOT_MAX). Please reduce its size.\033[0m" ; \
		exit 1; \
	 fi

###################
# draminit

# If CONFIG_STANDALONE_DRAMINIT=y, use draminit.img.
ifeq ($(CONFIG_STANDALONE_DRAMINIT), y)
DRAMINIT_IMG := ../draminit/bin/draminit.img

else
# Otherwise, link xboot with plf_dram.o

DWC_SRC_DIR = ../draminit/dwc/software/lpddr4/src
DWC_USER_DIR = ../draminit/dwc/software/lpddr4/userCustom
DWC = dwc_ddrphy_phyinit_
DWC_USER = dwc_ddrphy_phyinit_userCustom_
DRAMINIT_OBJ := ../draminit/dwc/dwc_dram.o
DRAMINIT_OBJ += ../draminit/dwc/dwc_umctl2.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)main.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)globals.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)sequence.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)restore_sequence.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)C_initPhyConfig.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)I_loadPIEImage.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)progCsrSkipTrain.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)setUserInput.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)getUserInput.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)cmnt.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)print.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)assert.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)print_dat.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)setMb.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)softSetMb.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)getMb.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)LoadPieProdCode.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)calcMb.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)G_execFW.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)H_readMsgBlock.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)mapDrvStren.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)D_loadIMEM.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)F_loadDMEM.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)storeIncvFile.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)storeMsgBlk.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)WriteOutMem.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)IsDbyteDisabled.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)initStruct.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)regInterface.o
DRAMINIT_OBJ += $(DWC_SRC_DIR)/$(DWC)setStruct.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC)setDefault.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)overrideUserInput.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)A_bringupPower.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)B_startClockResetPhy.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)customPreTrain.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)customPostTrain.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)E_setDfiClk.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)G_waitFwDone.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)H_readMsgBlock.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)J_enterMissionMode.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)io_write16.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)io_read16.o
DRAMINIT_OBJ += $(DWC_USER_DIR)/$(DWC_USER)saveRetRegs.o

# Use prebuilt obj if provided
CONFIG_PREBUILT_DRAMINIT_OBJ := $(shell echo $(CONFIG_PREBUILT_DRAMINIT_OBJ))
ifneq ($(CONFIG_PREBUILT_DRAMINIT_OBJ),)
DRAMINIT_OBJ := $(CONFIG_PREBUILT_DRAMINIT_OBJ)
endif
endif # CONFIG_STANDALONE_DRAMINIT

# build target
debug: DRAMINIT_TARGET:=debug

build_draminit:
	@echo ">>>>>>>>>>> Build draminit"
	$(MAKE) -C ../draminit $(DRAMINIT_TARGET) ARCH=$(ARCH) CROSS=$(CROSS) MKIMAGE=$(MKIMAGE)
	@echo ">>>>>>>>>>> Build draminit (done)"
	@echo ""

dwc:
	@echo ">>>>>>>>>>> Build dwc obj"
	$(MAKE) -C ../draminit/dwc ARCH=$(ARCH) CROSS=$(CROSS) PLATFROM=SP7350

	@echo ">>>>>>>>>>> Build dwc obj (done)"
	@echo ""

# Boot up
ASOURCES_START := arch/$(CPU_PATH)/start.S

ASOURCES = $(ASOURCES_START)
CSOURCES += xboot.c

CSOURCES += common/diag.c

# MON shell
ifeq ($(CONFIG_DEBUG_WITH_2ND_UART), y)
CSOURCES += romshare/regRW.c
endif
ifeq ($(MON), 1)
CFLAGS += -DMON=1
endif

# Common
CSOURCES += common/common.c common/bootmain.c common/stc.c
CSOURCES += common/string.c lib/image.c

# ARM code
CSOURCES += arch/$(CPU_PATH)/cpu/cpu.c arch/$(CPU_PATH)/cpu/interrupt.c lib/eabi_compat.c
ifeq ($(ARCH),arm)
empty :=
space += $(empty) $(empty)
arch/$(CPU_PATH)/cpu/cpu.o: CFLAGS:=$(subst -mthumb$(space),,$(CFLAGS))
endif

# Generic Boot Device
ifeq ($(CONFIG_HAVE_NAND_COMMON), y)
CSOURCES += nand/nandop.c nand/bch.c
endif

# mmu
CSOURCES += arch/arm/sp7350/cache.c

# Secure
CSOURCES += common/verify_image.c

# Parallel NAND
ifeq ($(CONFIG_HAVE_PARA_NAND), y)
CSOURCES += nand/para_nand.c
CSOURCES += nand/para_nand_hal.c
endif

# SPI NAND
ifeq ($(CONFIG_HAVE_SPI_NAND), y)
CSOURCES += nand/spi_nand.c
endif

# FAT
ifeq ($(CONFIG_HAVE_FS_FAT),y)
CSOURCES += fat/fat_boot.c
endif

# USB
ifeq ($(CONFIG_HAVE_USB_DISK), y)
CSOURCES += usb/ehci_usb.c
endif

# SNPS USB3
ifeq ($(CONFIG_HAVE_SNPS_USB3_DISK), y)
CSOURCES += usb/xhci_usb.c
endif

# MMC
ifeq ($(CONFIG_HAVE_MMC), y)
CSOURCES += sdmmc/drv_sd_mmc.c  sdmmc/hal_sd_mmc.c sdmmc/hw_sd.c
endif

# I2C
ifeq ($(CONFIG_HAVE_I2C), y)
CSOURCES += i2c/sp_i2c.c
endif

# ADC
ifeq ($(CONFIG_HAVE_ADC), y)
CSOURCES += adc/sp_adc.c
endif

# OTP
ifeq ($(CONFIG_HAVE_OTP), y)
CSOURCES += otp/sp_otp.c
endif

ifeq ($(CONFIG_HAVE_OTP_HS), y)
CSOURCES += otp/mon_rw_otp.c
endif

# CPIO
CSOURCES += cpio/cpio_sp7350.c

CSOURCES += draminit/dram_test.c

OBJS = $(ASOURCES:.S=.o) $(CSOURCES:.c=.o)

$(OBJS): prepare

$(TARGET): $(OBJS) a64bin
	@echo ">>>>> Link $@  "
	@$(CPP) -P $(CFLAGS) -x c $(LD_SRC) -o $(LD_GEN)
	$(CC) $(CFLAGS) $(OBJS) $(DRAMINIT_OBJ) -T $(LD_GEN) $(LDFLAGS) -o $(BIN)/$(TARGET) -Wl,-Map,$(BIN)/$(TARGET).map
	@$(OBJCOPY) -O binary -S $(BIN)/$(TARGET) $(BIN)/$(TARGET).bin
	@$(OBJDUMP) -d -S $(BIN)/$(TARGET) > $(BIN)/$(TARGET).dis

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

a64bin:
	@echo "Build a64 bin"
	@$(MAKE) -C arch/arm/sp7350/a64up/
	@$(CROSS)objcopy -I binary -O elf32-littlearm -B arm --rename-section .data=.a64bin arch/arm/sp7350/a64up/a64up.bin arch/arm/sp7350/a64up/a64bin.o

HSMK_BIN=../../build/tools/secure_hsm/secure/hsm_keys/hsmk.bin
HSMK_OBJ=../../build/tools/secure_hsm/secure/hsm_keys/hsmk.o

hsmk:
ifeq ($(SECURE),1)
	@echo "Build hsm key obj"
	@if [ ! -f $(HSMK_BIN) ];then \
		echo "Not found hsm key bin: $(HSMK_BIN)" ; \
		exit 1; \
	 fi
	@$(CROSS)objcopy -I binary -O elf32-littlearm -B arm --rename-section .data=.hsmk $(HSMK_BIN) $(HSMK_OBJ)
endif

#################
# dependency
.depend: $(ASOURCES) $(CSOURCES)
	@rm -f .depend >/dev/null
	@$(CC) $(CFLAGS) -MM $^ >> ./.depend
sinclude .depend

#################
# clean
.PHONY: clean
clean:
	@$(MAKE) -C arch/arm/sp7350/a64up/ clean
	@$(MAKE) -C warmboot clean
	@$(MAKE) -C ../draminit $(DRAMINIT_TARGET) ARCH=$(ARCH) CROSS=$(CROSS) $@
	@rm -rf .depend $(LD_GEN) $(OBJS) *.o *.d>/dev/null
	@if [ -d $(BIN) ];then \
		cd $(BIN) && rm -rf $(TARGET) $(TARGET).bin $(TARGET).map $(TARGET).dis $(TARGET).img $(TARGET).img.orig $(TARGET).sig >/dev/null ;\
	 fi;
	@echo "$@: done"
	@$(MAKE) -C ../draminit/dwc $(DRAMINIT_TARGET) ARCH=$(ARCH) CROSS=$(CROSS) $@
	@rm -rf $(OBJS) *.o *.d>/dev/null
	@echo "$@: done"

distclean: clean
	@rm -rf .config .config.old $(BIN)/v7
	@rm -f GPATH GTAGS GRTAGS
	@rm -rf $(BIN)
	@echo "$@: done"

#################
# configurations
.PHONY: prepare
prepare: dwc
	@mkdir -p $(BIN)

AUTOCONFH=tools/auto_config/auto_config_h
MCONF=tools/mconf

config_list=$(subst configs/,,$(shell find configs/ -maxdepth 1 -mindepth 1 -type f|sort))
$(config_list):
	@if [ ! -f configs/$@ ];then \
		echo "Not found config file for $@" ; \
		exit 1 ; \
	fi
	@make clean >/dev/null
	@echo "Configure to $@ ..."
	@cp configs/$@ .config

list:
	@echo "$(config_list)" | sed 's/ /\n/g'

auto_config:
	@echo "  [KCFG] $@.h"
	$(AUTOCONFH) .config include/$@.h
	$(AUTOCONFH) .config ../draminit/dwc/include/$@.h

chkconfig:
	@if [ ! -f .config ];then \
		echo "Please make XXX to generate .config. Find configs by: make list" ; \
		exit 1; \
	fi

config menuconfig:
	@$(MCONF) Kconfig

#################
# misc
r: clean all
pack:
	@make -C ../../ipack
