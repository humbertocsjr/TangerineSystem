include ../config.mk

OUT = 10404.img 1440.img
T = @../bin/tfs
B = @../bin/cfgboot

all: $(OUT)
	@true

clean: 
	@rm $(OUT) | true

%.img: $(wildcard ../bin/* ../kernel/*.elf ../kernel/*.bin ../boot/*.bin ../lib/*.lib ../config.*) Makefile
	@echo [IMG] $@
	$T $@ genfs $(patsubst %.img,%,$@) "Tangerine System"
	$T $@ bootsect ../boot/stage1.bin
	$T $@ bootloader ../boot/stage2.bin
	$T $@ mkdir /Applications
	$T $@ mkdir /System
	$T $@ mkdir /Library
	$T $@ mkdir /Users
	$T $@ mkdir /Volumes
	$T $@ add /System/LoaderStage1 ../boot/stage1.bin
	$T $@ add /System/LoaderStage2 ../boot/stage2.bin
	$T $@ add /System/Kernel ../kernel/kernel.bin
	$T $@ add /System/Modules ../config.mod
	$T $@ add /Library/c.lib ../lib/c.lib
	$B $@ "-disk $(BOOT_DISK) $(patsubst %,-load %,$(BOOT_MODULES))"
	


