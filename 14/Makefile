CC = gcc

final.img : Boot.img Sector2.img disk.img
	cat Boot.img Sector2.img disk.img > final.img

disk.img : main.img
	objcopy -O binary main.img disk.img

main.img :main.o function.o interrupt.o
	ld -melf_i386 -Ttext 0x10200 -nostdlib main.o function.o interrupt.o -o main.img

main.o : main.c
	gcc -c  -masm=intel -m32 -ffreestanding main.c -o main.o

function.o : function.c
	gcc -c -masm=intel -m32 -ffreestanding function.c -o function.o

interrupt.o : interrupt.c
	gcc -c -masm=intel -m32 -ffreestanding interrupt.c -o interrupt.o

Boot.img : Boot.asm
	nasm -f bin -o Boot.img Boot.asm

Sector2.img : Sector2.asm
	nasm -f bin -o Sector2.img Sector2.asm

clean :
	rm *.o *.img
	
