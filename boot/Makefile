OUT = stage1.bin stage2.bin


all: $(OUT)
	@true

clean:
	@rm $(OUT) | true

%.bin: %.asm $(wildcard src/*.asm)
	@echo [ASM] $@
	@nasm -f bin -o $@ $<