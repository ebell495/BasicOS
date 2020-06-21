CSOURCES = $(shell find kernel -type f -iname '*.c')
COBJECTS = $(foreach x, $(basename $(CSOURCES)), $(x).o)

ASOURCES = $(shell find kernel -type f -iname '*.asm')
AOBJECTS = $(foreach x, $(basename $(ASOURCES)), $(x).o)

BSOURCES = bootloader/boot1.asm bootloader/boot2.asm
BBINS = $(foreach x, $(basename $(BSOURCES)), $(x).bin)

EXTRAFILES = testImage.bmp

BOOTLOADER = bin/boot.bin
KERNEL = bin/kernel.bin

TARGET = image/os-image.img

TOOLS = fsTool/leanfs

GCC = gcc
GCCFLAGS = -Wall -fno-pie -fno-stack-protector -static -m32 -ffreestanding -c

NASM = nasm
BOOTFLAGS = -f bin
AOBJFLAGS = -f elf32

LD = ld
LDFLAGS = -Ttext 0x1500 -melf_i386 --oformat binary

OBJS = $(shell find -type f -iname '*.o')
BINS = $(shell find -type f -iname '*.bin')
IMG = $(shell find -type f -iname '*.img')

$(TARGET): $(BOOTLOADER) $(KERNEL) ${TOOLS}
	#cat $(BOOTLOADER) $(KERNEL) > $(TARGET)
	#dd if=/dev/zero of=$(TARGET) bs=1 count=1 seek=131071
	./fsTool/leanfs -s 3072 -b 3 -v basicos --raw=$(BOOTLOADER),0 --insert=$(KERNEL),$(EXTRAFILES) $(TARGET)
	#rm -f $(OBJS)
	#rm -f $(BINS)

$(BOOTLOADER): $(BBINS)
	cat $(BBINS) > $(BOOTLOADER)

$(KERNEL): bootloader/kernelEntry.o $(COBJECTS) $(AOBJECTS)
	ld $(LDFLAGS) -o $@ bootloader/kernelEntry.o $(AOBJECTS) $(COBJECTS)
	dd if=/dev/zero of=$(KERNEL) bs=1 count=1 seek=59391

%.bin: %.asm
	$(NASM) $(BOOTFLAGS) $^ -o $@

%.o: %.c
	$(GCC) $(GCCFLAGS) $^ -o $@

%.o: %.asm
	$(NASM) $(AOBJFLAGS) $^ -o $@

fsTool/leanfs: fsTool/Main.cpp fsTool/LEAN.cpp
	g++ fsTool/Main.cpp fsTool/LEAN.cpp -IfsTool/include -o fsTool/leanfs

clean:
	rm -f $(OBJS)
	rm -f $(BINS)
	rm -f $(IMG)
	rm -f $(TOOLS)