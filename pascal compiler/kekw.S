.bss
a: .space 4

.bss
b: .space 4

.bss
x: .space 40

.text
.global _main
 
_main:
 
xorl %eax, %eax
xorl %ebx, %ebx
 
_*op1.0:
 
movl $1, a
_*op1.1:
 
movl $5, x + 36
_*op1.2:
 
pushl $2
movl $5, %ebx
popl %eax
addl %ebx, %eax
pushl %eax
popl %eax
movl %eax, x + 16
_*op1.3:
 
pushl $5
pushl x + 20
popl %ebx
popl %eax
xorl %edx, %edx
mull  %ebx
pushl %eax
pushl x + 36
popl %ebx
popl %eax
subl  %ebx, %eax
pushl %eax
pushl $3
movl a, %ebx
popl %eax
xorl %edx, %edx
divl  %ebx
pushl %eax
popl %ebx
popl %eax
addl %ebx, %eax
pushl %eax
popl %eax
movl %eax, b
 
leave
ret

