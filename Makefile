M = @make --no-print-directory $@ -C

all clean: reqs
	@mkdir -p bin
	$M sdk
	$M boot
	$M lib
	$M kernel
	$M distro


tinycc:
	@chmod +x sdk/tinycc/configure
	@sdk/tinycc/configure --cpu=x86_64
	@make --no-print-directory all cross-i386 -C sdk/tinycc

install-tinycc:
	@make --no-print-directory install -C sdk/tinycc

clean-tinycc:
	@make --no-print-directory clean -C sdk/tinycc

reqs: config.h config.mk config.mod
	@true
config.h: 
	@make --no-print-directory config
config.mk: 
	@make --no-print-directory config
config.mod: 
	@make --no-print-directory config

clean-config:
	@rm config.h config.mk config.mod

run: all
	@qemu-system-i386  -debugcon stdio -drive if=ide,format=raw,index=0,media=disk,file=distro/10404.img

debug: all
	@bochs -debugger

config:
	@if [ -z "tcc" ]; then \
	echo "Error: TinyCC is not installed."; \
	echo "Run..: make clean-tinycc"; \
	echo "Run..: make tinycc"; \
	echo "Run..: sudo make install-tinycc"; \
	exit 1; \
	fi
	@if [ -z "ruby" ]; then \
	echo "Error: Ruby is not installed."; \
	exit 1; \
	fi
	@if [ -z "ruby" ]; then \
	echo "Error: Ruby Gems is not installed."; \
	exit 1; \
	fi
	@if [ -z "dialog" ]; then \
	echo "Error: Dialog is not installed."; \
	exit 1; \
	fi
	@if ! gem list "^mrdialog$$" -i > /dev/null; then \
	echo "Error: MRDialog Gem is not installed (sudo gem install mrdialog)."; \
	exit 1; \
	fi
	@chmod +x sdk/config/config.rb
	@./sdk/config/config.rb