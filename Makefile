sinclude .config

# Toolchain path
export PATH := ../../build/tools/armv5-eabi--glibc--stable/bin/:$(PATH)
CROSS   := armv5-glibc-linux-

BIN     := bin
TARGET  := xboot
CFLAGS   = -Os -Wall -g -nostdlib -fno-builtin -Iinclude
CFLAGS  += -ffunction-sections -fdata-sections
CFLAGS  += -mthumb -mthumb-interwork
LD_SRC   = boot.ldi
LD_GEN   = boot.ld
LDFLAGS  = -T $(LD_GEN) -L $(shell dirname `$(CROSS)gcc -print-libgcc-file-name`) -lgcc
LDFLAGS += -Wl,--gc-sections,--print-gc-sections


all:
	@echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
	@echo ">>>>>>>>>>> Build xBoot"
	@$(MAKE) v5_xb

v5_xb v7_xb: $(TARGET)
	@# 32-byte xboot header
	bash ./add_header.sh $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).img.orig
	@# print xboot size
	@sz=`du -sb bin/$(TARGET).img.orig | cut -f1` ; \
	    printf "xboot.img size = %d (hex %x)\n" $$sz $$sz

	@#FIXME: xboot.img = xboot.img.orig + draminit.img
	@mv $(BIN)/$(TARGET).img.orig $(BIN)/$(TARGET).img

# OUT
v5_xb: OUT:=$(BIN)
v7_xb: OUT:=$(BIN)/v7

# arch-specific CFLAGS
v5_xb: CFLAGS+=-march=armv5te
v7_xb: CFLAGS+=-march=armv7-a

# Boot up
ASOURCES_V5 := start.S
#ASOURCES_V7 := v7_start.S
ASOURCES = $(ASOURCES_V5) $(ASOURCES_V7)
CSOURCES += xboot.c

CSOURCES += common/diag.c

# MON shell
#ifeq ($(CONFIG_HAVE_MON_SHELL), y)
#CSOURCES += common/regRW.c
#endif

# Common
CSOURCES += common/common.c common/bootmain.c common/stc.c

# ARM code
CSOURCES += cpu/cpu.c lib/eabi_compat.c
space :=
space +=
cpu/cpu.o: CFLAGS:=$(subst -mthumb$(space),,$(CFLAGS))

# Generic Boot Device
ifeq ($(CONFIG_HAVE_NAND_COMMON), y)
CSOURCES += nand/nandop.c nand/bch.c
endif

# Parallel NAND
ifeq ($(CONFIG_HAVE_PARA_NAND), y)
CSOURCES += nand/nfdriver.c
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

# MMC
ifeq ($(CONFIG_HAVE_MMC), y)
CSOURCES += sdmmc/drv_sd_mmc.c  sdmmc/hal_sd_mmc.c sdmmc/hw_sd.c
endif

OBJS = $(ASOURCES:.S=.o) $(CSOURCES:.c=.o)

$(OBJS): prepare

$(TARGET): $(OBJS)
	@echo ">>>>> Build $@ code"
	$(CROSS)cpp -P $(CFLAGS) $(LD_SRC) $(LD_GEN)
	$(CROSS)gcc $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(OUT)/$(TARGET) -Wl,-Map,$(OUT)/$(TARGET).map
	$(CROSS)objcopy -O binary -S $(OUT)/$(TARGET) $(OUT)/$(TARGET).bin
	$(CROSS)objdump -d -S $(OUT)/$(TARGET) > $(OUT)/$(TARGET).dis

%.o: %.S
	$(CROSS)gcc $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CROSS)gcc $(CFLAGS) -c -o $@ $<


#################
# dependency
.depend: $(ASOURCES) $(CSOURCES)
	@rm -f .depend >/dev/null
	@$(CROSS)gcc $(CFLAGS) -MM $^ >> ./.depend
sinclude .depend

#################
# clean
.PHONY:clean
clean:
	@rm -rf .depend $(LD_GEN) $(OBJS) *.o >/dev/null
	@rm -rf  $(BIN)/v5
	@cd $(BIN) && rm -rf $(TARGET) $(TARGET).bin $(TARGET).map $(TARGET).dis $(TARGET).img $(TARGET).rcf >/dev/null
	@echo "$@: done"

distclean: clean
	@rm -rf .config .config.old $(BIN)/v7
	@rm -f GPATH GTAGS GRTAGS
	@echo "$@: done"

#################
# configurations
.PHONY: prepare
prepare: auto_config
	mkdir -p $(OUT)

AUTOCONFH=tools/auto_config_h
MCONF=tools/mconf

config_list=$(subst configs/,,$(shell find configs/ -maxdepth 1 -mindepth 1 -type f))
$(config_list):
	@if [ ! -f configs/$@ ];then \
		echo "Not found config file for $@" ; \
		exit 1 ; \
	fi
	@make clean >/dev/null
	@echo "Configure to $@ ..."
	@cp configs/$@ .config

list:
	@echo "$(config_list)"

auto_config: chkconfig
	@echo "  [KCFG] $@.h"
	$(AUTOCONFH) .config include/$@.h

chkconfig:
	@if [ ! -f .config ];then \
		echo "Please make XXX to generate .config. Find configs by make list." ; \
		exit 1; \
	fi

config menuconfig:
	@$(MCONF) Kconfig


#################
# misc
r: clean all
pack:
	@make -C ../../ipack
