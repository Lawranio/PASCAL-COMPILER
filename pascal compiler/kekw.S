.bss
a: .space 4

.bss
b: .space 4

.bss
e: .space 4

.bss
c: .space 1

.text
.global _main
 
_main:
 
xor1 %eax, %eax
xor1 %ebx, %ebx
 
_*op1.0:
 
movl $1, a
_*op1.1:
 
pushl aa
movl $2, %ebx
popl %eax
addl %ebx, %eax
pushl %eax
popl %eax
movl %eax, b
_*op1.2:
 
pushl bb
movl a, %ebx
popl %eax
subl  %ebx, %eax
pushl %eax
popl %eax
movl %eax, e
 
leave
ret

