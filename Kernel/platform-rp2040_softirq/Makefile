export PICO_SDK_PATH = /home/anichang/Documents/pico-sdk

build/fuzix.elf: ../version.c build/Makefile
	$(MAKE) -C build

../version.c:
	$(MAKE) -C .. TARGET=rpipico_rt version.c

build/Makefile: CMakeLists.txt $(wildcard ../*.[chS]) $(wildcard ../*/*.[chS])
	mkdir -p build
	(cd build && cmake ..)

image:: world uf2conv
	./update-flash.sh
	../../Standalone/mkftl -s 1952 -e 0x1000 -g 10 build/root.flash.img -o build/root.flash.ftl
	arm-none-eabi-objcopy -I binary -O elf32-littlearm --change-section-vma .data=0x10018000 build/root.flash.ftl build/root.flash.elf
	./build/uf2conv build/root.flash.ftl build/root.flash.uf2 0x10018000

clean:
	rm -rf build
	$(MAKE) -C ../../Library/libs -f Makefile.armm0 clean
	$(MAKE) -C ../../Applications/util -f Makefile.armm0 clean
	$(MAKE) -C ../../Applications/V7/cmd -f Makefile.armm0 clean
	$(MAKE) -C ../../Applications/V7/cmd/sh -f Makefile.armm0 clean
	$(MAKE) -C ../../Applications/games -f Makefile.armm0 clean
	$(MAKE) -C ../../Applications/levee -f Makefile.armm0 clean
	$(MAKE) -C ../../Applications/cave -f Makefile.armm0 clean
	$(MAKE) -C ../../Applications/cursesgames -f Makefile.armm0 clean
	$(MAKE) -C ../../Standalone clean

world: build/fuzix.elf
	$(MAKE) -C ../../Library/libs -f Makefile.armm0
	$(MAKE) -C ../../Applications/util -f Makefile.armm0
	$(MAKE) -C ../../Applications/V7/cmd -f Makefile.armm0
	$(MAKE) -C ../../Applications/V7/cmd/sh -f Makefile.armm0
	$(MAKE) -C ../../Applications/games -f Makefile.armm0
	$(MAKE) -C ../../Applications/levee -f Makefile.armm0
	$(MAKE) -C ../../Applications/cave -f Makefile.armm0
	$(MAKE) -C ../../Applications/cursesgames -f Makefile.armm0
	$(MAKE) -C ../../Standalone

uf2conv: tools/uf2conv.c
	cc -O -g -o build/$@ $<

