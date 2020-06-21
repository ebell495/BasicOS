#!/bin/bash

qemu-system-x86_64 -drive file="image/os-image.img",format=raw -m 128M -serial file:SERIAL.BIN