.bss
a: .space 4

b: .space 4

c: .space 4

i: .space 4

x: .space 40

.text
.global _main
 
_main:
 
xorl %eax, %eax
xorl %ebx, %ebx
 
_op1.0:
 
movl $1, a
 
_op1.0_end:
_op1.1:
 
pushl a
pushl $5
popl %ebx
popl %eax
cmp %ebx, %eax
jle  _nope1_
pushl a
movl $1, %ebx
popl %eax
addl %ebx, %eax
pushl %eax
popl %eax
movl %eax, a
jmp _end1_
 _nope1_:
jmp _op1.1_end:
 _end1_:
 
_op1.1_end:
 
leave
ret

