# Interrupts Handler

## Prerequisite Registers and Function
1. sstatus register
2. sscratch register
3. sie register
4. sip register

### sstatus
- The SIE(Supervisor Interrupts Enable) bit of sstatus controls whether if interrupts are enabled or not.
- The SPIE(Supervisor Previous Interrupts Enable) controls the previous state
The Working is here
- Upon ecall or other interrupt of exception, The SIE is cleared to zero and SPIE is set equal to SIE.
- Upon sret the SPIE is copied into SIE and that is it.

### sscratch
- Stores either zero or trapframe
- Cleared the very last step before interrupts are enabled in the kernel initially

### sie
- sie have, seie (Supervisor External Interrupts Enable), stie(Supervisor timer Interrupts Enable) and SSIE(Supervisor Software Interrupts Enable)

### sip
- Pretty useful in debugging and check whether interrupts are even occuring, automatically cleared by hardware most of the times.


There are mainly two kind of Interrupts Handler
1. The U-Mode to S-Mode Interrupts Handler
2. The S-Mode to S-Mode Interrupts Handler (Used for Nested Interrupts)

The Main Lazy Condition we set here is the sscratch register.
If the interrupt was from U-Mode we would have the trapframe address in sscratch
If the Interrupts was from S-Mode, The sscratch would be zero.

But We have to make sure that the sscratch is zero, which is crucial to manage this system

## U-Mode to S-Mode interrupts
1. Step 1: When we drop into the interrupt handler the SPIE(Supervisor Previous Interrupt Enable) will be set and SIE will be cleared
2. Step 2: sscratch checking will happen, by swapping sscratch and t0 and beq branching assembly
3. Step 3: After entering the U-mode to S-mode handler, swap sscratch and t0 again to restore
4. Step 4: swap sscratch and sp and start pushing values into the trapframe
5. Step 5: While pushing when we push the final sp into the trapframe, we must push the value stored in sscratch since we swapped sscratch and sp
6. Step 6: For allowing nested interrupts we make sscratch zero so that S-mode to S-Mode interrupts work.
7. Step 7: Enable Interrupts
8. Step 8: Do the handling
9. Step 9: Disable interrupts
10. Step 10: Pop of the trapframe and then pop the values into the correct registers, make sure sp is popped last.
11. Step 11: Perfrom sret

## S-Mode to S-Mode
1. Step 1: sscratch is already zero. SPIE = 1 and SIE = 0, Interrupts are already disabled
2. Step 2: Push into the frame , no requirment to change SP as it is in the kernel stack of the current user process.
3. Step 3: handle the interrupts
4. Step 4: Pop off the stuff
5. Perform sret.

In S-Mode to S-Mode we don't have interrupts enabled anaywhere, since it would sometimes overflow the stack.
