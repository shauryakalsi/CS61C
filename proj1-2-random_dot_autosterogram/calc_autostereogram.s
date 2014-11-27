.text

# Generates an autostereogram inside of buffer
#
# Arguments:
#     autostereogram (unsigned char*)
#     depth_map (unsigned char*)
#     width
#     height
#     strip_size
calc_autostereogram:

        # Allocate 5 spaces for $s0-$s5
        # (add more if necessary)
        addiu $sp $sp -28
        sw $s0 0($sp)
        sw $s1 4($sp)
        sw $s2 8($sp)
        sw $s3 12($sp)
        sw $s4 16($sp)
        sw $s5 20($sp)
        sw $s6 24($sp)

        # autostereogram
        lw $s0 28($sp)
        # depth_map
        lw $s1 32($sp)
        # width
        lw $s2 36($sp)
        # height
        lw $s3 40($sp)
        # strip_size
        lw $s4 44($sp)

        # YOUR CODE HERE #
	li $s5 0 #i = 0  
    	li $s6 0 #j = 0
    	addiu $sp $sp -4 #store $ra onto the stack
    	sw $ra 0($sp) 
#temp variables that need to be saved: 
Outer_For:
    beq $s5 $s2 End #if i == width, return
Inner_For:
	blt $s5 $s4 Random #if i < S, branch Random
	#getting i,j value of depth_map
	mul $t0 $s6 $s2  #row*width
    	addu $t0 $t0 $s5 #row*width + column
	
	#for depth map
	addu $t1 $s1 $t0 #address of offset for depth_map
	lbu $t2 0($t1) #storing value of depth_map i,j
	addu $t2 $t2 $s5 #t2 = i + depth(i,j)
	subu $t2 $t2 $s4 #t2 = i + depth(i,j) - Strip_size
	
	#calculating I((i + depth(i,j)-S,j)
	mul $t3 $s6 $s2
	addu $t3 $t3 $t2
	
	#calculating value at I((i + depth(i,j)-S,j)
	addu $t3 $t3 $s0 #address of offset for I((i + depth(i,j)-S,j)
	lbu $t3 0($t3) #storing value of I((i + depth(i,j)-S,j)
	
	#storing value of I((i + depth(i,j)-S,j) into I(i,j)
	addu $t4 $s0 $t0  #address of offset for I(i,j)
	sb $t3 0($t4)  #storing value of I(i,j)


	
	
Loop_condition:
	# Loop conditions 
	addi $s6 $s6 1 # j = j+1
	bne $s6 $s3 Inner_For #if j != height, repeat Inner loop
	addu $s6 $0 $0        #j set back to 0
	addi $s5 $s5 1        #i = i + 1
	j Outer_For
	
	
Random: 
	jal lfsr_random #gives random number to be entered in stereogram
	andi $v0 $v0 0xFF #converts to 8-bit
	
	#get (i,j) location of autostereogram
	mul $t0 $s6 $s2  #row*width
    	addu $t0 $t0 $s5 #row*width + column
    	addu $t1 $s0 $t0 #offset address of I(i,j)
    	
    	sb $v0 0($t1) #stores random value into I(i,j)
	j Loop_condition

End:
	lw $ra 0($sp)
	
        lw $s0 4($sp)
        lw $s1 8($sp)
        lw $s2 12($sp)
        lw $s3 16($sp)
        lw $s4 20($sp)
        lw $s5 24($sp)
        lw $s6 28($sp)
        addiu $sp $sp 32
        jr $ra
