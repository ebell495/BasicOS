;Function print
;Prints the string pointed to in bx
temp: times 3 dw 0

print:		
	pusha
	mov ah, 0x0e		;Tell that bios that we want to teletype
	
	pStart01:
	
		mov al, [bx]	;move the value of the string
		cmp al, 0		;See if we have hit a zero
		je pend01		;End the loop
		int 0x10		;Interrupt the bios to print a character
		
		add bx, 1		;Increment the address by one
		jmp pStart01	;Loop
		
	pend01:
	
	popa
	ret
	
;Function println
;Prints the string pointed to in bx then ends with a newline and carrage return
printnl:
	pusha
	call print
	
	mov ah, 0x0e	;Tell that bios that we want to teletype	
	mov al, 0x0A	;Ascii for newline
	int 0x10
	
	mov al, 0x0D	;Ascii for carrage return
	int 0x10
	
	popa
	ret
	
;Function printHex
;Prints the representation of the value in dx
printHex:
	pusha

	mov cl, 12			;This is the counter, starts at 12 to signify the amount is needed to shift at the beginning
						;ex (1111 0000 0000 0000) >> 12
	mov bx, temp		;Move the address that we allocated to bx
	phLoop:
		
		mov ax, dx		;move the data into ax	
		shr ax, cl		;Shift it right by the counter
		and ax, 1111b	;and make sure everything else is gone
		
		add ax, '0'		;Bring it into the world of ascii	
		cmp ax, '9'		;Compare it to the last ascii digit
		jle ph01		;continue if it is a digit
		
		add ax, 7		;Otherwise increase it to the alphabet
		
		ph01:
		mov [bx], ax	;Move the character to the address from earlier
		add bx, 1		;Increment the address
		
		cmp cl, 0		;See if the counter has reached zero
		je phLoopExit	;If it has it is done
		sub cl, 4		;If not, reduce it by four(For four bits)

		jmp phLoop		;Loop
		
	phLoopExit:
	
	mov ah, byte 0x00	;To null-terminate the final string
	mov [bx], ah		;Move the null-terminator to the final string
	
	mov bx, temp		;Move the address of the string to bx
	call printnl		;Call the print string function
	
	popa
	ret
	
;Reads the number of sectors stored in dh from the disk stored in dl
;At the sector in cl (Sectors start at 0x01[The Bootloader])
;Sectors are also 512 bytes
;Puts the data read at the memory address stored in bx

diskLoad:
	pusha
	
	push dx
	
	mov ah, 0x02
	mov al, dh
	mov ch, 0x00
	mov dh, 0x00
	
	int 0x13
	
	jc diskErrorA
	
	pop dx
	cmp dh, al
	jne diskErrorB
	
	popa
	ret

;These represent the error codes of the disk
diskErrorA:
	mov dx, ax
	call printHex
	mov bx, DISK_ERR_STR_A
	call printnl
	jmp $
	
diskErrorB:
	mov dx, ax
	call printHex
	mov bx, DISK_ERR_STR_B
	call printnl
	jmp $
	
DISK_ERR_STR_A:
	db 'Disk Read ErrorA', 0
	
DISK_ERR_STR_B:
	db 'Disk Read ErrorB', 0

