#Vadim Darchuk 316920974 and Yotam Alter 302955679

.data
b: .space 8
d: .space 200
e: .space 8


strBuff: .space 200
.text
Program:

li $v0,5
syscall
sw $v0, a
li $v0,5
syscall
sw $v0, b
lw $t0, Label0
sw $t0, d
li $v0,4
lw $a0, d
syscall
lw $t0, a
lw $t1, b
slt $t2, $t0, $t1
beq $t2,$0,ElseLabel6
lw $t1, a
li $v0, 1
lw $a0, $t1
syscall

j EndLabel6
ElseLabel6:
lw $t0, b
li $v0, 1
lw $a0, $t0
syscall

EndLabel6:

