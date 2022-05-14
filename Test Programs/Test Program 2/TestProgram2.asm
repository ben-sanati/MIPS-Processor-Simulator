beq $zero, $zero, main			#branch to main
procedureFunc:
	addi $sp, $sp -4		#create room on the stack to push
	sw $s0, 0($sp)			#save h onto stack
	add $s0, $a0, $a1		#a + b
	add $v0, $s0, $zero		#store h into result
	lw $s0, 0($sp)			#load h onto stack
	addi $sp, $sp, 4		#pop h off stack
 	jr $ra				#return

DoIf:
	addi $a1, $zero, 12	#b = 12
  	jal procedureFunc
  	j SkipElse

main: 
	addi $a0, $zero, 15		#a = 15
	
	#begin if else statement
	addi $t5, $zero, 0
	beq $a0, $t5, DoIf		#if a == 5  (which it isnt)
 	addi $a1, $zero, -12		#b = -12 
 	jal procedureFunc
 	SkipElse:
 	add $s0, $v0, $zero		#h = procedure function return value
 	

#arguments a and b in $a0 and $a1 respectively
#result is in $s0 (should be 3)
