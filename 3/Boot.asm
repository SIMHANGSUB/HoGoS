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
	mov bx, 0 ; 0x1000:0000 주소로 읽어 => 물리주소 0x10000

	mov ah, 2 ; 디스크에 있는 데이터를 es:bx의 주소로
	mov al, 1 ; 1섹터를 읽을 것이다
	mov ch, 0 ; 0번째 실린더
	mov cl, 2 ; 2번째 섹터부터 읽기 시작한다
	mov dh, 0 ; 헤드는 0
	mov dl, 0 ; 플로피 디스크 읽기 
	int 13h
	
	jc read ; 에러나면 다시
	
jmp 0x1000:0

times 510-($-$$) db 0
dw 0xAA55