build:
	nasm -f bin "bootloader/boot1.asm" -o "bin/bootloader/boot1.bin"
	nasm -f bin "bootloader/boot2.asm" -o "bin/bootloader/boot2.bin"
	nasm "kernel/asm/kernelEntry.asm" -f elf32 -o "bin/kernel/kernelEntry.o"
	cat "bin/bootloader/boot1.bin" "bin/bootloader/boot2.bin" > "bin/bootloader/bootloader.bin"
	gcc -Wall -fno-pie -static -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -nostartfiles -nodefaultlibs -g -c "kernel/kernel.c" -o "bin/kernel/kernel.o"
	gcc -Wall -fno-pie -static -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -nostartfiles -nodefaultlibs -g -c "kernel/lib/hwio.c" -o "bin/kernel/hwio.o"
	gcc -Wall -fno-pie -static -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -nostartfiles -nodefaultlibs -g -c "kernel/lib/display.c" -o "bin/kernel/display.o"
	gcc -Wall -fno-pie -static -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -nostartfiles -nodefaultlibs -g -c "kernel/lib/ps2k.c" -o "bin/kernel/ps2k.o"
	gcc -Wall -fno-pie -static -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -nostartfiles -nodefaultlibs -g -c "kernel/lib/memlib.c" -o "bin/kernel/memlib.o"
	ld -melf_i386 -o "bin/kernel/kernel.bin" -Ttext 0x2000 "bin/kernel/kernelEntry.o" "bin/kernel/kernel.o"  "bin/kernel/display.o" "bin/kernel/hwio.o" "bin/kernel/ps2k.o" "bin/kernel/memlib.o" --oformat binary
	cat "bin/bootloader/bootloader.bin" "bin/kernel/kernel.bin"> "image/os-image.img"
	dd if=/dev/zero of=image/os-image.img bs=1 count=1 seek=19922943

clean:
	rm -r bin
	mkdir bin
	mkdir bin/bootloader
	mkdir bin/kernel