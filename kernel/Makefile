include ../config.mk
OUT = kernel.bin
KERNEL_MODULES = $(BUS_MODULES) $(IO_MODULES) $(DISK_MODULES) $(VIDEO_MODULES) $(KEYBOARD_MODULES) $(MOUSE_MODULES) $(NETWORK_MODULES)
SRC = $(wildcard *.asm *.c src/*.asm src/*.c) $(patsubst %,modules/%.c,$(KERNEL_MODULES))
OBJ = $(patsubst %.asm,%.o,$(patsubst %.c,%.o,$(SRC)))

all: $(OUT)
	@true

clean:
	@rm -f $(OUT) $(OBJ) *.o video/*.o src/*.o io/*.o keyboard/*.o disk/*.o bus/*.o | true

%.o: %.c $(wildcard *.h ../include/*.h src/*.h modules/*.h ../config.h)
	@echo [CC ] $@
	@tcc -m32 -nostdinc -I../include/ -c -o $@ $<

%.o: %.asm
	@echo [ASM] $@
	@nasm -f elf32 -o $@ $<

%.elf: $(OBJ)
	@echo [LD ] $@
	@tcc -m32 -nostdlib -Wl,-whole-archive -r $^ -o $@

%.bin: %.elf ../lib/c.lib
	@echo [LD ] $@
	@tcc -m32 -nostdlib -L../lib -Wl,-Ttext,0x100000 -Wl,--oformat,binary -static $^  -o $@
	@ndisasm -b32 kernel.bin -e 20 -o 0x100014 > kernel.lst
