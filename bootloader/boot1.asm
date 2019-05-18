[org 0x7c00]			;Because the actual bootloader is loaded here, we can
						;Offset like this

mov [BOOT_DRIVE], dl
						
mov bp, 0x0FFF			;This sets up the stack far away from the loaded bootloader
mov sp, bp

mov bx, 0x1000			;Location to store the data
mov dh, 2				;Number of sectors to read(4) or 
mov dl, [BOOT_DRIVE]
mov cl, 0x02
call diskLoad

mov dl, [BOOT_DRIVE]

jmp 0x1000				;The location where we loaded the 2nd stage of the bootloader

jmp $

;Functions, make sure that the program does not go past this point
;Use a jmp $ to stop here

;Load memorymap to the memeo
%include "bootloader/library/rstdio.asm"
;Data
BOOT_DRIVE: dw 0

times 510-($-$$) db 0 	;Fill the rest of the file with 0's to make a boot sector

db 0x55 				;The magic boot number
db 0xAA
