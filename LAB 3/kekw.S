.bss
a: .space 4

.bss
b: .space 4

.bss
e: .space 4

.text
.global _main
 
_main:
 
xor1 %eax, %eax
xor1 %ebx, %ebx
 
_*op1.0:
 
 
leave
ret

