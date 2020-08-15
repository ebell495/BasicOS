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

global exp0
global exp1
global exp2
global exp3
global exp4
global exp5
global exp6
global exp7
global exp8
global exp9
global exp10
global exp11
global exp12
global exp13
global exp14
global exp16
global exp17
global exp18
global exp19
global exp20
global exp30

extern exception_handler

global irq80_yield
global irq81_enablePreemtion
global irq82_disablePreemtion
 
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
extern irq_yield_preempt_handler
extern irq_enablePreemtion_handler
extern irq_disablePreemtion_handler
 
irq80_yield:

  cli

  push ss
  push es
  push fs
  push gs
  push ds
  push eax
  push ebx
  push ecx
  push edx
  push edi
  push esi
  push ebp
  push esp

  ;Put the start address of the registers onto the stack
  ;This will help facilitate a context switch
  mov edx, esp
  push edx

  call irq_yield_preempt_handler

  pop edx

  mov esp, edx


  pop esp
  pop ebp
  pop esi
  pop edi
  pop edx
  pop ecx
  pop ebx
  pop eax
  pop ds
  pop gs
  pop fs
  pop es
  pop ss

  sti
  iret

irq81_enablePreemtion:
  pushad
  call irq_enablePreemtion_handler
  popad
  iret

irq82_disablePreemtion:
  pushad
  call irq_disablePreemtion_handler
  popad
  iret

irq0:
  cli

  push ss
  push es
  push fs
  push gs
  push ds
  push eax
  push ebx
  push ecx
  push edx
  push edi
  push esi
  push ebp
  push esp

  ;Put the start address of the registers onto the stack
  ;This will help facilitate a context switch
  mov edx, esp
  push edx

  call irq0_handler

  pop edx

  mov esp, edx

  mov al, 0x20
  out 0x20, al

  pop esp
  pop ebp
  pop esi
  pop edi
  pop edx
  pop ecx
  pop ebx
  pop eax
  pop ds
  pop gs
  pop fs
  pop es
  pop ss

  sti
  iret

irq1:
  pushad

  mov al, 0x20
  out 0x20, al 
  popad
  iret
irq2:
  pushad

  mov al, 0x20
  out 0x20, al 
  popad
  iret
irq3:
  pushad

  mov al, 0x20
  out 0x20, al 
  popad
  iret
irq4:
  pushad

  mov al, 0x20
  out 0x20, al 
  popad
  iret
irq5:
  pushad

  mov al, 0x20
  out 0x20, al 
  popad
  iret
irq6:
  pushad

  mov al, 0x20
  out 0x20, al 
  popad
  iret
irq7:
  pushad

  mov al, 0x20
  out 0x20, al 
  popad
  iret

irq8:
  pushad

  mov al, 0x20
  out 0x20, al
  out 0xa0, al 
  popad
  iret
irq9:
  pushad

  mov al, 0x20
  out 0x20, al
  out 0xa0, al 
  popad
  iret
irq10:
  pushad

  mov al, 0x20
  out 0x20, al
  out 0xa0, al 
  popad
  iret
irq11:
  pushad

  mov al, 0x20
  out 0x20, al
  out 0xa0, al 
  popad
  iret
irq12:
  pushad

  mov al, 0x20
  out 0x20, al
  out 0xa0, al 
  popad
  iret
irq13:
  pushad

  mov al, 0x20
  out 0x20, al
  out 0xa0, al 
  popad
  iret
irq14:
  pushad
  mov al, 0x20

  out 0x20, al
  out 0xa0, al 
  popad
  iret
irq15:
  pushad
  mov al, 0x20

  out 0x20, al
  out 0xa0, al 
  popad
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

exp0:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 0
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp1:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 1
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp2:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 2
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp3:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 3
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp4:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 4
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp5:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 5
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp6:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 6
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp7:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 7
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp8:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 8
  push eax
  mov eax, [esp + 12]
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  add esp, 4
  iret

exp9:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 9
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp10:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 10
  push eax
  mov eax, [esp + 12]
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  add esp, 4
  iret

exp11:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 11
  push eax
  mov eax, [esp + 12]
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  add esp, 4
  iret
exp12:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 12
  push eax
  mov eax, [esp + 12]
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  add esp, 4
  iret

exp13:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 13
  push eax
  mov eax, [esp + 12]
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  add esp, 4
  iret

exp14:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 14
  push eax
  mov eax, [esp + 12]
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  add esp, 4
  iret

exp16:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 16
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp17:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 17
  push eax
  mov eax, [esp + 12]
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  add esp, 4
  iret

exp18:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 18
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp19:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 19
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp20:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 20
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret

exp30:
  pushad
  ;Store the exception number and pass as argument
  mov eax, 30
  push eax
  mov eax, 0
  push eax
  call exception_handler
  pop eax
  pop eax
  popad
  iret