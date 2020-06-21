times 446-($-$$) db 0 	;Fill the rest of the file with 0's to get the the area for the mbr

;Partition Entry one
db 0x80				;This is an active partition

					;Starts at
db 0x00				;Head 0
db 0x04				;Sector 4
db 0x00			    ;Cylinder 0
					;This is LBA of 3

db 0xEA				;Partition Type (EAh is for a LEAN partition)

					;The partition ends at
db 0x00				;Head 0
db 0x00				;Sector 0
db 0x04				;Cylinder 4

dd 0x03				;LBA Start
dd 0x100			;LBA End
;End of the first parition entry

;Partition Entry two
times 16 db 0 

;Partition Entry three
times 16 db 0 

;Partition Entry four
times 16 db 0 

