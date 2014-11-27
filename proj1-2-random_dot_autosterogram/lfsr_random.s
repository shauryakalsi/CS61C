.data

lfsr:
        .align 4
        .half
        0x1

.text

# Implements a 16-bit lfsr
#
# Arguments: None
lfsr_random:

        la $t0 lfsr
        lhu $v0 0($t0)

        # YOUR CODE HERE #
    li $t1 0
    li $t8 16
Loop:   
    
    srl $t2 $v0 2
    srl $t3 $v0 3
    srl $t4 $v0 5
    xor $t5 $t4 $t3     #(reg >> 5) ^ (reg >> 3)
    xor $t6 $v0 $t2     #(reg >> 0) ^ (reg >> 2)
    xor $t7 $t6 $t5     #highest = (reg >> 0) ^ (reg >> 2) ^ (reg >> 3) ^ (reg >> 5)
    srl $t2 $v0 1       # (reg >> 1)
    sll $t3 $t7 15      # (highest << 15)
    andi $t3 $t3 0xFFFF #ands the lower half with 1's and upper half with 0's
    or $v0 $t2 $t3     #reg = (reg >> 1) | (highest << 15);
    
    
    addiu $t1 $t1 1 
    bne $t1 $t8 Loop    
        la $t0 lfsr
        sh $v0 0($t0)
        jr $ra
