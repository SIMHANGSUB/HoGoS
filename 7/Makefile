CC = gcc

final.img : Boot.img Sector2.img disk.img
	cat Boot.img Sector2.img disk.img > final.img

disk.img : main.img
	objcopy -O binary main.img disk.img

main.img :main.o
	ld -melf_i386 -Ttext 0x10200 -nostdlib main.o -o main.img

main.o : main.c
	gcc -c -m32 -ffreestanding main.c -o main.o

Boot.img : Boot.asm
	nasm -f bin -o Boot.img Boot.asm

Sector2.img : Sector2.asm
	nasm -f bin -o Sector2.img Sector2.asm

clean :
	rm *.o