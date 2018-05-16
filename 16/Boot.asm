[org 0]
[bits 16]

jmp 0x07C0:start

start:
mov ax, cs
mov ds, ax
mov es, ax

read:
	mov ax, 0x1000
	mov es, ax
	mov bx, 0 ; 0x1000:0000 주소로 읽어 => 물리주소 0x10000

	mov ah, 2 ; 디스크에 있는 데이터를 es:bx의 주소로
	mov al, 20 ; 20섹터를 읽을 것이다 MAX = 64
	mov ch, 0 ; 0번째 실린더 MAX 1024
	mov cl, 2 ; 2번째 섹터부터 읽기 시작한다
	mov dh, 0 ; 헤드는 0 MAX 16
	mov dl, 0 ; 플로피 디스크 읽기 
	int 13h
	
	jc read ; 에러나면 다시

	mov dx, 0x3F2 ;플로피디스크 드라이브의
	xor al, al	; 모터를 끈다
	out dx, al 
	
	cli

	; 인터럽트 코드 시작

	mov al, 0x11	;pic 초기화
	out 0x20, al	;마스터 PIC
	dw 0x00eb, 0x00eb	;jmp $+2, jmp $+2
	out 0xA0, al	;슬레이브 PIC
	dw 0x00eb, 0x00eb
	
	mov al, 0x20;마스터 PIC  인터럽트 시작점
	out 0x21, al
	dw 0x00eb, 0x00eb
	mov al, 0x28
	out 0xA1, al
	dw 0x00eb, 0x00eb
	
	mov al, 0x04	;마스터 PIC의 IRQ 2번에
	out 0x21, al	;슬레이브 PIC이 연결되어 있다.
	dw 0x00eb, 0x00eb
	mov al, 0x02	;슬레이브 PIC이 마스터 PIC이
	out 0xA1, al	;IRQ 2번에 연결되어 있다.
	dw 0x00eb, 0x00eb
	
	mov al, 0x01	; 8086모드를 사용한다.
	out 0x21, al
	dw 0x00eb, 0x00eb
	out 0xA1, al
	dw 0x00eb, 0x00eb
	
	mov al, 0xFF	;슬레이브 PIC의 모든 인터럽트를
	out 0xA1, al 	;막아둔다.
	dw 0x00eb, 0x00eb
	mov al, 0xFB	;마스터 PIC의 IRQ 2번을 제외한
	out 0x21, al	;모든 인터럽터를 막아둔다.

	; 인터럽트 코드 끝

lgdt[gdtr]
	
mov eax, cr0
or eax, 1
mov cr0, eax
	
jmp $+2
nop
nop

mov bx, 0x10
mov ds, bx
mov es, bx
mov fs, bx
mov gs, bx
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