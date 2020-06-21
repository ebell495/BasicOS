; mode_info_struct:
; 	.attrib: dw 0
; 	.window_a: db 0
; 	.window_b: db 0
; 	.granularity: dw 0
; 	.window_size: dw 0
; 	.segment_a: dw 0
; 	.segment_b: dw 0
; 	.win_func_ptr: dd 0
; 	.pitch: dw 0
; 	.width: dw 0
; 	.height: dw 0
; 	.w_char: db 0
; 	.y_char: db 0
; 	.planes:  db 0
; 	.bpp: db 0
; 	.banks: db 0
; 	.memory_mode: db 0
; 	.bank_size: db 0
; 	.image_page: db 0
; 	.res0: db 0
; 	.red_mask: db 0
; 	.red_position: db 0
; 	.green_mask: db 0
; 	.green_position: db 0
; 	.blue_mask: db 0
; 	.blue_position: db 0
; 	.reserved_mask: db 0
; 	.reserved_position: db 0
; 	.direct_color_attributes: db 0
; 	.framebuffer: dd 0
; 	.off_screen_mem_off: dw 0
; 	.off_screen_mem_size: dw 0
; 	.res1: times 206 db 0

vbe_error_str: db 'Error getting vba mode', 0
vbe_set_mode_error_str: db 'Error setting vba mode', 0
vbe_init_pm_table_error_str: db 'Error getting pm table', 0

;Get information about the requested mode
;Mode should be in cx
;Loads the address of the info table to address 0xF0000
vbe_get_mode_info:
	pusha 
	;and cx, 0x3FFF
	mov ax, 0x2000
	mov es, ax;
	mov ax, 0x4F01
	mov di, 0x0
	int 0x10
	cmp ax, 0x004F
	jne .vbe_get_mode_error

	mov dx, [es:di + 18]
	call printHex

	mov dx, [es:di + 20]
	call printHex

	mov dx, [es:di]
	call printHex
	mov dx, [es:di]
	call printHex

	popa
	ret

.vbe_get_mode_error:
	mov bx, vbe_error_str
	call print
	jmp $


;Sets the vbe mode
;The mode should be in bx
vbe_set_mode:
	pusha
	or bx, 0x4000	;Sets the 14th bit for Linear Framebuffer
	and bx, 0x7FFF	;0's the 15th bit 
	mov ax, 0x4F02
	int 0x10
	cmp ax, 0x004F
	jne .vbe_set_mode_error
	
	popa
	ret

.vbe_set_mode_error:
	mov bx, vbe_set_mode_error_str
	call print
	jmp $

;Loads the pm table to 0xF210
vbe_init_pm_table:
	pusha
	mov ax, 0x1000
	mov es, ax;
	mov ax, 0x4F0A
	mov bx, 0x100
	mov di, bx
	mov bx, 0
	int 0x10
	cmp ax, 0x004F
	jne .vbe_init_pm_table_error

	popa
	ret


.vbe_init_pm_table_error:
	mov bx, vbe_init_pm_table_error_str
	call print
	jmp $


