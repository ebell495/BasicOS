[org 0x1000]			  	;Tell the assembler that we are loaded at the memory location 0x1000
[bits 16]

KERNEL_OFFSET equ 0x2000
mmap_ent times 4 dw 0

mov [BOOT_DRIVE], dl		;Passed the boot drive to the 

call loadKernel				;Read in and load the kernel to the kernel offset

call do_e820

call switchToPM

jmp $

loadKernel:
	mov bx, KERNEL_OFFSET	;Store the kernel data in the memory location at this label
	mov dh, 30				;Load in 30 Sectors or 14KB of kernel data
	mov dl, [BOOT_DRIVE]	;Load from the boot drive that the bios provided earlier and was passed from the first stage
	mov cl, 0x06 			;Kernel is in the 6th sector (1: 1st stage, 2-5: Second Stage)
	call diskLoad
	
	ret

switchToPM:
	cli
	lgdt [gdt_descriptor]
	
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax
	
	mov ax, DATA_SEG
	mov dx, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	jmp CODE_SEG:init_pm
	
%include "bootloader/library/rstdio.asm"

;Loads in the memory map for the kernel to read and interpret
do_e820:
	pusha
	xor di, di					;Zero out the offset pointer register
	mov bx, 0x400				;Prepare 0x400 for the segment register
	mov es, bx					;This means that the mem address is ex * 0x10 + di
	
	xor ebx, ebx				;ebx must be 0 to start
	xor bp, bp					;keep an entry count in bp
	mov edx, 0x0534D4150		;Place "SMAP" into edx
	mov eax, 0xe820
	mov [es:di + 20], dword 1	;force a valid ACPI 3.X entry
	mov ecx, 24					;ask for 24 bytes
	int 0x15
	jc .failed					;carry set on first call means "unsupported function"
	mov edx, 0x0534D4150		;Some BIOSes apparently trash this register?
	cmp eax, edx				;on success, eax must have been reset to "SMAP"
	jne .failed
	test ebx, ebx				;ebx = 0 implies list is only 1 entry long (worthless)
	je .failed
	jmp .jmpin
	
.e820lp:
	mov eax, 0xe820				;eax, ecx get trashed on every int 0x15 call
	mov [es:di + 20], dword 1	;force a valid ACPI 3.X entry
	mov ecx, 24					;ask for 24 bytes again
	int 0x15
	jc .e820f					;carry set means "end of list already reached"
	mov edx, 0x0534D4150		;repair potentially trashed register
	
.jmpin:
	jcxz .skipent				;skip any 0 length entries
	cmp cl, 20					;got a 24 byte ACPI 3.X response?
	jbe .notext
	test byte [es:di + 20], 1	;if so: is the "ignore this data" bit clear?
	je .skipent
	
.notext:
	mov ecx, [es:di + 8]		;get lower uint32_t of memory region length
	or ecx, [es:di + 12]		;"or" it with upper uint32_t to test for zero
	jz .skipent					;if length uint64_t is 0, skip entry
	inc bp						;got a good entry: ++count, move to next storage spot
	add di, 24
	
.skipent:
	test ebx, ebx				;if ebx resets to 0, list is complete
	jne .e820lp
	
.e820f:
	mov bx, 0x7000
	mov [bx], bp				;store the entry count at 0x7000
	clc							;there is "jc" on end of list to this point, so the carry must be cleared
	mov bx, MEM_READ_SUC
	call printnl
	popa
	ret
	
.failed:			; "function unsupported" error exit
	mov bx, MEM_DETECT_ERROR
	call printnl
	jmp $

DISK_ERR_STR_A:
	db 'Disk Read ErrorA', 0
MEM_DETECT_ERROR: db 'Error Detecting Memory', 0
MEM_READ_SUC: db 'Detected Memory Sucessfully', 0
BOOT_DRIVE: dw 0

;------------------------------------------------------------------
;Keep this section for protected mode calls
;Also keep the gdt here because of protected mode
;Any real mode functions should be kept above this section
;Any protected mode functions should be kept below this section after the gdt include
;------------------------------------------------------------------

%include "bootloader/library/gdt.asm"
;This is some code for when we get into protected mode
[bits 32]	;We now have 32 bits to work with

VIDEO_MEMORY equ 0xb8000	;Make the video memory point to the second line of the screen
WHITE_ON_BLACK equ 0x0F

printStringPM:
	pusha
	mov edx, VIDEO_MEMORY

printStringPMLoop:
	mov al, [ebx]			;Store the character in al
	mov ah, WHITE_ON_BLACK	;Store the color information in ah
	
	cmp al, 0				;Hit a null character
	je psPMDone				;Jump to the end
	
	mov [edx], ax			;Move the character information to the video memory
	
	add ebx, 1				;Increment the character information
	add edx, 2				;Increment the video memory information
	
	jmp printStringPMLoop

psPMDone:
	popa
	ret
	
init_pm:
	
	mov ebp, 0x9FBFF 	;Location of the stack for the kernel to use
						;We know this is a good memory location
	mov esp, ebp
	
	call BEGIN_PM
	
BEGIN_PM:			;This is the start of all the important protected mode operations like starting the kernel and clearing the screen
	call clScreen
	
	mov ebx, PM_STRING
	call printStringPM
	
	call is_A20_on
	
	call KERNEL_OFFSET
	
	jmp $

clScreen:
	pusha
	mov ebx, 0x7d0
	mov edx, VIDEO_MEMORY
	
	clScreenStart:
	cmp ebx, 0
	je clSDone
	
	mov al, 0x0
	mov ah, 0x0F
	
	mov [edx], ax
	add edx, 2
	sub ebx, 1
	
	jmp clScreenStart
	
clSDone:	
	popa
	ret
	
is_A20_on:   
	pushad
	mov edi,0x112345  ;odd megabyte address.
	mov esi,0x012345  ;even megabyte address.
	mov [esi],esi     ;making sure that both addresses contain diffrent values.
	mov [edi],edi     ;(if A20 line is cleared the two pointers would point to the address 0x012345 that would contain 0x112345 (edi)) 
	cmpsd             ;compare addresses to see if the're equivalent.
	popad
	jne A20_on        ;if not equivalent , A20 line is set.
	mov ebx, A20_NS
	call printStringPM
	ret               ;if equivalent , the A20 line is cleared.
 
A20_on:
	mov ebx, A20_SET
	call printStringPM
	ret


PM_STRING db "Entered protected mode sucessfully", 0
A20_SET db "A20 line is set", 0
A20_NS db "A20 line is not set", 0

;Allocate quite a bit of space (2KB) to the second stage for future proofing
times 2048-($-$$) db 0