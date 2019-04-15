# BasicOS
A very simple operating system written in Assembly and C


# Features/Functions
- Multi-stage bootloader
- e820 Memory Map reading
- Basic I/O from ps2 port
- Simple 80x25 Text display interface

# Building
Right now, this only builds on unix/linux based environments
## Build Toolchain
- Make
- NASM
- gcc
- ld

To build, run `make` in a terminal

# Running
To run, you can use a virtual machine like 
- [QEMU](https://www.qemu.org/) (Recommended)
- [bochs](http://bochs.sourceforge.net/)
- [VirtualBox](https://www.virtualbox.org/)

Configuration depends on the software you choose to use
### Using QEMU
This is the recommended emulator to run the OS

To run, execute the following in the root directory of the project
```shell
qemu-system-x86_64 -drive format=raw,file="image/os-image.img" -m 32M
```

### Using VirtualBox
VirtualBox doesn't support booting off a raw disk image, so to get around this we convert the image into a vmdk

To convert the raw images, execute the following commands
```shell
VBoxManage convertfromraw os-image.img testimage.vmdk --format vmdk

#This sets the uuid of the new vmdk, change the uuid to a new one every time this command is run
VBoxManage internalcommands sethduuid testimage.vmdk 0a467099-8628-48d9-8315-1647b9251001
```

Now you can create a new VM and use the newly created vmdk as the disk to use
