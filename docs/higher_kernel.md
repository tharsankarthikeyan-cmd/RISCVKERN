# Higher Half Kernel in RISC V
Higher Half Kernel is the industry standard
## Learning Essential Compiler Terms and Definitions
1. Essential Sections
2. Global Pointer Optimization in RISC V
3. Compiler Setting and push, norelax and pop

### Essential Sections
- .text - This is the section where the code goes
- .data - This is the section where the variables are stored
- .bss - This is section where the uninitialized variables goes, including the stack
- .rodata - This is the section where the read only constant data goes
- ALIGN(4K) - Aligns the section to a 4KiB boundary
- AT(Enter Address Here) - AT the address of
- ADDR(Enter Section) - The current address of the given section
- . means here in the current address
- . += (some address) means add an address to the current address and further use that address to generate variable and function addresses.

### Global Pointer
- Global Pointer is a special feature in RISC V called linker Relaxation
- Global Pointer is used for the quicker access of the variables within the +- 2KiB Range
- According to the Linker setting the compiler generates the obj files which either include variable Optimization via GP or not
- The global pointer is initialized at the center of the RAM and the variables are accessed via the GP pointer + or - 2KiB range
- wrt to Paging, The Global Pointer Optimization has some very standard issues
- Issue: The Global Pointer if initialized the compiler will start using Optimization but if not then it would work normally
- If the Global Pointer is set to a virtual address, then one must turn on the GP only after the Paging Setup is done
- for that we must discuss the next section

### Compiler Settings
- Compiler Settings can be saved using
```
.option push
```

- Compiler Optimization can be brought back
```
.option pop
```

- > [!IMPORTANT]
> The Compiler Optimization for Global Pointer can be turned off via given below compiler directive
```
.option norelax
```

- The compiler Optimization is now turned off

- Finally connecting all of these the general workflow must be
- Step 1: Do .option push to save the current status of the compiler
- Step 2: Do .option norelax to stop gp based linker Relaxation
- Step 3: Do the kernel paging, bare minimum paging must be done
- Step 4: Enable Paging via the SATP and sfence.vma/
- Step 5: load the GP properly
- Step 5: Do .option pop to bring back the original compiler settings with the Optimization
- Step 6: tail k_main to jump to the kernel

# Important Concepts Regarding Assembly Addressing
1. PC-Relative Addressing for Large Address Spaces in RISC V General Purpose Kernels
2. GP-Relative Addressing for Variable Access and Linker Relaxation/
3. Canonical and Non Canonical Addressing
3. Using the GNU Debugger
## PC-Relative
- In this addressing the addresses loaded are PC (+-) Some Bytes
- In a higher half kernel one must trick the linker into thinking that it is in the higher address space when the PC is executing in the Lower Address
- Futhermore when one accesses the address of one of the variables the Assembler Converts Access information into a PC-Relative address
- This is because instead of reading the memory directly one can just fetch something from PC(+-) some bytes, Which is Easier
- Hence PC - Relative Address is Used
- But This causes us some essential problem, After setting up paging one can jr t0 into the higher address space with the address of the kernel_main function.
- The Debugger thinks wrt to the Linker Causing it to say that the address of the kernel_main is just normally the Higher Half, Which is exactly what we want but, The Main Issue is given below
- When we use an la instruction to load the address of the kernel_main function, Due to PC Relative Addressing the Address will still be in the Lower Half due to PC, Messing up the JUMP to the higher half.
- We can think about adding 0xFFFFFFC000000000 but issue arises due to Canonical and Non Canonical(Refer the Discussion below)

- The Solution, One must be able to access the absolute memory address of the kernel main with respect to the linker
- Given below code illustrates that
```
kernel_main_addr:
  .dword kernel_main

la t0, kernel_main_addr
ld, t1, 0(t0)
```

- ld - meaning Load Word Loaded the Address of kernel_main ass a variable from the kernek_main_addr list
- This is a nice Solution for medany mcmodel type

## GP-Relative
- We have already discusses this above, GP is turned of by ".option norelax"

## Canonical and Non Canonical
- In a RV64 system, It is very very inefficient to have 64 Address lines to access the address space and very hard to decode the given addr
- Therefore we only ever using 56 bits for fetch and that too, In Paging SV39 we only use 39 bits and a total of 512GB RAM Access
- The remaining bits [63:39] are all either 1 or 0
- A Canonical Address is something which has all the bits from [63:39] match the 38th bit in Sv49 and the 46th bit in Sv47
- Only a Canonical Address is accepted by the MMU for access of RAM, all others are Ignored and Faulted or Trapped by the CPU and MMU
- Why you ask
- Programmer are extremely intelligent creatures and do not care about the pain of other programmers, so That would just use the top bits to store metadata causing a program to be non universal and particular to one system
- This causes a headache of other programmers and it becomes a loop of headache and comes back to you anyways, So RISC designers said fuck it all the bits unused must match the that last MSB bit of the Current Paging Scheme



