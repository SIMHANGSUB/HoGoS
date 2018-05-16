[org 0]
[bits 16]

jmp 0x07C0:start

start:
mov ax, cs
mov ds, ax


mov ax, 0xB800
mov es, ax

mov byte[es:0], 'h'
mov byte[es:1], 0x09
mov byte[es:2], 'i'
mov byte[es:3], 0x09

read:
	mov ax, 0x1000
	mov es, ax
	mov bx, 0 ; 0x1000:0000 주소로 읽어 => 물리주소 0x10000

	mov ah, 2 ; 디스크에 있는 데이터를 es:bx의 주소로
	mov al, 1 ; 1섹터를 읽을 것이다
	mov ch, 0 ; 0번째 실린더
	mov cl, 2 ; 2번째 섹터부터 읽기 시작한다
	mov dh, 0 ; 헤드는 0
	mov dl, 0 ; 플로피 디스크 읽기 
	int 13h
	
	jc read ; 에러나면 다시

	mov dx, 0x3F2 ;플로피디스크 드라이브의
	xor al, al	; 모터를 끈다
	out dx, al 
	
	cli

lgdt[gdtr]
	
mov eax, cr0
or eax, 1
mov cr0, eax
	
jmp $+2
nop
nop

mov bx, DataSegment
mov ds, bx
mov es, bx
mov ss, bx

jmp dword CodeSegment:0x10000


gdtr:
dw gdt_end - gdt - 1
dd gdt+0x7C00

gdt:

	dd 0,0 ; NULL 세그
	CodeSegment equ 0x08
	dd 0x0000FFFF, 0x00CF9A00 ; 코드 세그
	DataSegment equ 0x10
	dd 0x0000FFFF, 0x00CF9200 ; 데이터 세그
	VideoSegment equ 0x18
	dd 0x8000FFFF, 0x0040920B ; 비디오 세그

gdt_end:

times 510-($-$$) db 0
dw 0xAA55