OUT = ../c.lib
SRC = $(wildcard string/*.c string/*.asm)
OBJ = $(patsubst %.asm,%.o,$(patsubst %.c,%.o,$(SRC)))

all: $(OUT)
	@true

clean:
	@rm -f $(OUT) $(OBJ) | true


%.o: %.c $(wildcard *.h ../../include/*.h ../config.h)
	@echo [CC ] $@
	@tcc -m32 -nostdinc -rdynamic -I../../include/ -I. -c -o $@ $<

%.o: %.asm
	@echo [ASM] $@
	@nasm -f elf32 -o $@ $<

%.lib: $(OBJ) Makefile
	@echo [LD ] $@
	@tcc -m32 -ar rcs $@ $(OBJ)
