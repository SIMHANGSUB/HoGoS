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
	mov bx, 0 ; 0x1000:0000 �ּҷ� �о� => �����ּ� 0x10000

	mov ah, 2 ; ����ũ�� �ִ� �����͸� es:bx�� �ּҷ�
	mov al, 30 ; 20���͸� ���� ���̴� MAX = 64
	mov ch, 0 ; 0��° �Ǹ��� MAX 1024
	mov cl, 2 ; 2��° ���ͺ��� �б� �����Ѵ�
	mov dh, 0 ; ������ 0 MAX 16
	mov dl, 0 ; �÷��� ����ũ �б� 
	int 13h
	
	jc read ; �������� �ٽ�

	;mov dx, 0x3F2 ;�÷��ǵ���ũ �����̺���
	;xor al, al	; ���͸� ����
	;out dx, al 
	
	cli

	; ���ͷ�Ʈ �ڵ� ����

	mov al, 0x11	;pic �ʱ�ȭ
	out 0x20, al	;������ PIC
	dw 0x00eb, 0x00eb	;jmp $+2, jmp $+2
	out 0xA0, al	;�����̺� PIC
	dw 0x00eb, 0x00eb
	
	mov al, 0x20;������ PIC  ���ͷ�Ʈ ������
	out 0x21, al
	dw 0x00eb, 0x00eb
	mov al, 0x28
	out 0xA1, al
	dw 0x00eb, 0x00eb
	
	mov al, 0x04	;������ PIC�� IRQ 2����
	out 0x21, al	;�����̺� PIC�� �����Ǿ� �ִ�.
	dw 0x00eb, 0x00eb
	mov al, 0x02	;�����̺� PIC�� ������ PIC��
	out 0xA1, al	;IRQ 2���� �����Ǿ� �ִ�.
	dw 0x00eb, 0x00eb
	
	mov al, 0x01	; 8086���带 �����Ѵ�.
	out 0x21, al
	dw 0x00eb, 0x00eb
	out 0xA1, al
	dw 0x00eb, 0x00eb
	
	mov al, 0xFF	;�����̺� PIC�� ���� ���ͷ�Ʈ��
	out 0xA1, al 	;���Ƶд�.
	dw 0x00eb, 0x00eb
	mov al, 0xFB	;������ PIC�� IRQ 2���� ������
	out 0x21, al	;���� ���ͷ��͸� ���Ƶд�.

	; ���ͷ�Ʈ �ڵ� ��

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

	dd 0,0 ; NULL ����
	CodeSegment equ 0x08
	dd 0x0000FFFF, 0x00CF9A00 ; �ڵ� ����
	DataSegment equ 0x10
	dd 0x0000FFFF, 0x00CF9200 ; ������ ����
	VideoSegment equ 0x18
	dd 0x8000FFFF, 0x0040920B ; ������ ����

gdt_end:

times 510-($-$$) db 0
dw 0xAA55