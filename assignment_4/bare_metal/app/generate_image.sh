#!/bin/sh

arm-linux-gnueabi-as -mcpu=arm926ej-s -g startup.s -o startup.o
arm-linux-gnueabi-gcc -c -mcpu=arm926ej-s app.c -o app.o
arm-linux-gnueabi-ld -T app.ld -Map=app.map app.o startup.o -o app.elf
arm-linux-gnueabi-objcopy -O binary app.elf app.bin
mkimage -A arm -C none -O linux -T kernel -d app.bin -a 0x00100000 -e 0x00100000 app.uimg

cat ../images/u-boot.bin ../images/app.uimg > flash.bin

printf "bootm 0x%X\n" $(expr $(stat -c%s ../images/u-boot.bin) + 65536)
