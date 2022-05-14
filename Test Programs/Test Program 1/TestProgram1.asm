beq $zero, $zero, main			#branch to main
If:
	addi $t2, $zero, 0			#initialize value of i
	addi $s1, $zero, 0			#increment = 0
	addi $t3, $zero, 5			#loop 5 times
	ForLoop:
		beq $t3, $t2, ExitFor
		add $s1, $s1, $s0			#increment += sum
		addi $t2, $t2, 1			#increment i
		j ForLoop

main:
	addi $t0, $zero, 15				#x = 15
	addi $t1, $zero, 10				#y = 10
	add $s0, $t0, $t1				#sum = x + y
	addi $t5, $zero, 25
	beq $s0, $t5, If				#if(sum == 25)
	ExitFor:

#sum stored in $s0 (should be 25)
#double stored in $s1 (should be 125)
