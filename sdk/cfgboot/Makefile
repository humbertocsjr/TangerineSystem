

all: ../../bin/cfgboot
	@true
	
clean:
	@rm ../../bin/cfgboot | true

../../bin/cfgboot: cfgboot.c
	@echo [CC ] $@
	@cc -g -o $@ cfgboot.c
