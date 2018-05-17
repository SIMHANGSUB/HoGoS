CodeSegment equ 0x08
DataSegment equ 0x10
VideoSegment equ 0x18

[org 0x10000]
[bits 32]

START:
	mov bx, DataSegment
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx
	mov ss, bx
	lea esp, [START]
	

	mov edi,0
	mov esi, msg
	call printf

	jmp dword CodeSegment:0x10200

printf:
	push eax
	push es
	mov ax, VideoSegment ; ºñµð¿À
	mov es, ax
	
printf_loop:

	mov al, byte [esi]
	mov byte [es:edi], al
	inc edi
	mov byte [es:edi], 0x09
	inc esi
	inc edi
	or al, al
	jz printf_end
	jmp printf_loop

printf_end:
	pop es
	pop eax
	ret

msg db 'Call printf',0

times 512-($-$$) db 0