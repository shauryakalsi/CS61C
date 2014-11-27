.text

# Decodes a quadtree to the original matrix
#
# Arguments:
#     quadtree (qNode*)
#     matrix (void*)
#     matrix_width (int)
#
# Recall that quadtree representation uses the following format:
#     struct qNode {
#         int leaf; 0
#         int size; 4
#         int x;    8
#         int y;    12
#         int gray_value;  16
#         qNode *child_NW 20 , *child_NE 24, *child_SE 28, *child_SW 32;
#     }

quad2matrix:

        # YOUR CODE HERE #
    
    # $a0 : quadtree
    # $a1 : matrix
    # $a2: width
    
    addiu $sp $sp -12     #creates space on stack for $ra
    sw $ra 0($sp)         #stores $ra on stack
    sw $s0 4($sp)         #stores qNode address
    sw $s1 8($sp)	  #stores matrix address
    
    
    #sw $s1 8($sp)         #stores x value
    #sw $s2 12($sp)        #stores y value
    #sw $s3 16($sp)        #stores correct address in matrix
    #sw $s4 20($sp)        #stores gray_value
    #sw $a0 24($sp)        #stores $a0?
    
    move $s0 $a0
    move $s1 $a1	
    lw $t0 0($s0)        #store leaf value into $t0
    beq $t0 1 Store      #if leaf value is 1, i.e. it is leaf node
    
    #else recursive call
    lw $a0 20($s0)
    move $a1 $s1
    #addiu $a0 $a0 20
    #lw $a0 0($a0)
    jal quad2matrix         #recusrive call on qNW
    #lw $a0 24($sp)
    #addiu $a0 $a0 24
    lw $a0 24($s0)
    move $a1 $s1
    jal quad2matrix     #recursive call on qNE
    lw $a0 28($s0)
    move $a1 $s1
    #addiu $a0 $a0 28
    #lw $a0 0($a0)
    jal quad2matrix     #recursive call on qSE
    lw $a0 32($s0)
    move $a1 $s1
    #addiu $a0 $a0 32
    #lw $a0 0($a0)
    jal quad2matrix         #recursive call on qSW
End:     
        lw $ra 0($sp)         #loads $ra from stack
        lw $s0 4($sp)
        lw $s1 8($sp)
        #lw $s1 8($sp)
        #lw $s2 12($sp)
        #lw $s3 16($sp)
        #lw $s4 20($sp)
        #lw $a0 24($sp)
        addiu $sp $sp 12      #frees space
       
        jr $ra

Store:
    #store value in matrix
    lw $t1 8($s0)        #loads x value from qNode
    lw $t2 12($s0)       #loads y value from qNode
    lw $t3 4($s0)        #stores value of size
    addu $t6 $t1 $t3     #t6 = x + size
    addu $t5 $t2 $t3     #t5 = y + size
    
    #LOOPS THROUGH SIZE OF CHILD
    #USING SIZE VALUE OF QNODE
Outer_Loop:
    beq $t1 $t6 End
Inner_Loop:
    mul $t7 $t2 $a2      # t7 = y * width
    addu $t7 $t7 $t1     # t7 = t7 + x
    addu $t3 $t7 $a1     # t3 stores offset address in matrix
    lw $t4 16($s0)       #loads gray_value from qNode
    andi $t4 $t4 0xFF    #gives lower 8 bits
    sb $t4 0($t3)        #stores value in correct (hopefully) address in matrix
    addiu $t2 $t2 1
    bne $t2 $t5 Inner_Loop
    lw $t2 12($s0)
    addiu $t1 $t1 1
    j Outer_Loop
    
    