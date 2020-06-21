[bits 32]
[extern main]
_start:
call main	;This is the main in the kernel file
jmp $