[bits 32]
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

global testInt
 
global load_idt
 
extern irq0_handler
extern irq1_handler
extern irq2_handler
extern irq3_handler
extern irq4_handler
extern irq5_handler
extern irq6_handler
extern irq7_handler
extern irq8_handler
extern irq9_handler
extern irq10_handler
extern irq11_handler
extern irq12_handler
extern irq13_handler
extern irq14_handler
extern irq15_handler
extern irq_yield_preempt
 
testInt:
  pushad
  call irq_yield_preempt
  mov al, 0x20
  out 0xa0, al
  out 0x20, al
  popad
  iret

irq0:
  cli
  pushad
  call irq0_handler
  popad
  sti
  iret
 
irq1:
  pushad
  call irq1_handler
  popad
  iret
 
irq2:
  pushad
  call irq2_handler
  popad
  iret
 
irq3:
  pushad
  call irq3_handler
  popad
  iret
 
irq4:
  pusha
  call irq4_handler
  popa
  iret
 
irq5:
  pusha
  call irq5_handler
  popa
  iret
 
irq6:
  pusha
  call irq6_handler
  popa
  iret
 
irq7:
  pusha
  call irq7_handler
  popa
  iret
 
irq8:
  pusha
  call irq8_handler
  popa
  iret
 
irq9:
  pusha
  call irq9_handler
  popa
  iret
 
irq10:
  pusha
  call irq10_handler
  popa
  iret
 
irq11:
  pusha
  call irq11_handler
  popa
  iret
 
irq12:
  pusha
  call irq12_handler
  popa
  iret
 
irq13:
  pusha
  call irq13_handler
  popa
  iret
 
irq14:
  pusha
  call irq14_handler
  popa
  iret
 
irq15:
  pusha
  call irq15_handler
  popa
  iret
 
load_idt:
  cli
	mov edx, [esp + 4]
	lidt [edx]
	sti
  nop
  nop
  nop
  nop
  nop
  nop
	ret