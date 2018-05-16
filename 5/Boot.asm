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
	mov bx, 0 ; 0x1000:0000 �ּҷ� �о� => �����ּ� 0x10000

	mov ah, 2 ; ��ũ�� �ִ� �����͸� es:bx�� �ּҷ�
	mov al, 1 ; 1���͸� ���� ���̴�
	mov ch, 0 ; 0��° �Ǹ���
	mov cl, 2 ; 2��° ���ͺ��� �б� �����Ѵ�
	mov dh, 0 ; ���� 0
	mov dl, 0 ; �÷��� ��ũ �б� 
	int 13h
	
	jc read ; �������� �ٽ�

	mov dx, 0x3F2 ;�÷��ǵ�ũ ����̺���
	xor al, al	; ���͸� ����
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

	dd 0,0 ; NULL ����
	CodeSegment equ 0x08
	dd 0x0000FFFF, 0x00CF9A00 ; �ڵ� ����
	DataSegment equ 0x10
	dd 0x0000FFFF, 0x00CF9200 ; ������ ����
	VideoSegment equ 0x18
	dd 0x8000FFFF, 0x0040920B ; ���� ����

gdt_end:

times 510-($-$$) db 0
dw 0xAA55