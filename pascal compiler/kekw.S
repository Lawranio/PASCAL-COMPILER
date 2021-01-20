.bss
a: .space 4

b: .space 4

c: .space 4

d: .space 4

.text
.global _main
 
_main:
 
xorl %eax, %eax
xorl %ebx, %ebx
 
_op1.0:
 
movl $10, a
 
_op1.0_end:
_op1.1:
 
movl $1, b
 
_op1.1_end:
_op1.2:
 
movl $0, d
 
_op1.2_end:
_op1.3:
 
xorl %eax, %eax
movl $4, %ecx
 
loop_m_1:
 
_op2.0:
 
pushl b
pushl $3
popl %ebx
popl %eax
cmp %ebx, %eax
jle  _nope1_
movl $2225, a
jmp _end1_
 _nope1_:
pushl d
movl $10, %ebx
popl %eax
addl %ebx, %eax
pushl %eax
popl %eax
movl %eax, d
 _end1_:
 
_op2.0_end:
 
loop loop_m_1
 
_op1.3_end:
 
leave
ret

