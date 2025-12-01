

all: ../../bin/tfs
	@true
	
clean:
	@rm ../../bin/tfs | true

../../bin/tfs: tfs.c tfs.h
	@echo [CC ] $@
	@cc -g -o $@ tfs.c
