[org 0]
[bits 16]

jmp 0x07C0:start

start:

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
	
jmp 0x1000:0

times 510-($-$$) db 0
dw 0xAA55