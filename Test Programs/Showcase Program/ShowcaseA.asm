addi $t0, $zero, 0			#initialize value of i
addi $t1, $zero, 1			#n = 1
addi $t2, $zero, 0			#squareResult = 0
addi $t3, $zero, 0x10010000		#offset = 0
addi $s0, $zero, 99			#set max value of i 
LoopForA:
	beq $t0, $s0, ExitA
	add $t2, $t2, $t1		#squareResult = squareResult + n
	addi $t1, $t1, 2		#n = n + 2
	addi $t0, $t0, 1		#increment i
	sw $t2, 0($t3)
	addi $t3, $t3, 4		#offset += 4
	j LoopForA
ExitA:
addi $t6, $zero, 0x10010000
addi $t6, $t6, 36
lw $t5, 0($t6)