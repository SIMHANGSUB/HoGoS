[org 0x10000]
[bits 16]

mov ax, 0xB800
mov es, ax

mov byte[es:4], 'i'
mov byte[es:5], 0x09
mov byte[es:6], 'h'
mov byte[es:7], 0x09

jmp $

times 512-($-$$) db 0