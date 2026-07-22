# User Processes
A User Process is the code that runs on top of the kernel and its memory and other important things are managed by the kernel
A User Process talks to the kernel using syscall or ecalls(In RISCV), which is then handled by the trap and exception handlers

## Creation of the First User Process
Requirement: Page Frame Allocator

A user process requires some essential structs and functions before its creation
Given below are the essential parts and their role in the user function
1. New Root Page Tables
- Every Process has it own virtual address space mapped differently to the Physical RAM
- Whenever a process is created the kernel root page table is copied to only map the kernel's essential functions
- After the process program is loaded and execution has begun, Demand Paging(Lazy Paging) comes in, in order to create a new 4KB pages and map the address of the page to the new root pages table of the particular process, This creation and allocation only occurs when a Page Fault Occurs

The Copying Function Must have essential functionality
i. Each Page Table should 4KB to handle, 512* (64 bit) entries.
ii. They must be allocated using the `page_alloc()` function
iii. Given below is an example of the copying function based on recursion
If the entry is an addr the function follows a route where the function allocates and new page and then maps the entries of the page and return the address
If the entry is not an addr the entry is simply copied and but with different privlesges for U-Mode
There are some quirks which would be discussed after the code
```c

// Given the address of the original root page table of the kernel we return a copied page tables root table address
void* copy_page_data(void* page_entry_addr){
  // Create a New Page and then add the page offset so that it can be written to
  uint64_t* new_tab = (uint64_t*)((uintptr_t)page_alloc(4096) + (uintptr_t)0xFFFFFFC000000000ULL);

  // We loop the given root page table entries
  for(uint64_t i = 0; i < 512; i++){

    // We take the current entry one among 512 Entry of each table
    uint64_t current_entry = ((uint64_t*)page_entry_addr)[i];

    // If the given entry is pointer to an address we must walk it for we call the function again recursively
    if((current_entry & 0xF) == 0x1){
      
      // We take the address out of the current_entry using bit manipulation
      current_entry = ((current_entry >> 10) & 0xFFFFFFFFFFF) << 12;

      // Call the function recursively to the same for the given address
      void* next_page_entry = copy_page_data((void*)((uintptr_t)current_entry+(uintptr_t)0xFFFFFFC000000000ULL));

      // When the recursive call return the address of the new root page table to which it had copied to then
      // we take the return addresses and subtract the offset to get the physical address and make the entry into the root main page Table
      new_tab[i] = (((uintptr_t)next_page_entry - (uintptr_t)0xFFFFFFC000000000ULL) >> 2) | 0x01;
    }

    // If the entry is a simple leaf entry
    else{

      // Take the current entry and remove the flags
      current_entry = ((current_entry >> 10) & 0xFFFFFFFFFFF) << 10;

      // Add the same to the new root page table with the correct flags
      new_tab[i] = ((uint64_t)current_entry) | 0x0F;
    }
  }
  // Return the page address in order to avoid infinite recursion
  return (void*)(new_tab);
}
```

Diagram of the current setup

```text
Process 1                                | Process 2                                  | Physica Memory
------------ ==> Absolute Higher Address | ------------- ==> Absolute Higher Address  | ------------ ==> Absolute Higher Address
                                         |                                            | User Process Code 3 mapped here physically
Kernel Exists Here Virtually             | Kernel Exist Here Virtually                | User Process Code 2 mapped here physically
------------ ==> Kernel Begins           | ------------- ==> Kernel Begins            | User Process Code 5 mapped here physically
                                         |                                            | User Process Code 1 mapped here physically
                                         |                                            | User Process Code 4 Mapped Here physically
User Process Code 3                      |                                            | ------------> Kernel Ends
User Process Code 2                      | User Process Code 5                        | Kernel Exist Here Physical, both kernel virt is mapped here.
User Process Code 1                      | User Process Code 4                        | ------------ ==> Kernel Begins
------------ ==> Absolute Lower Address  | ------------- ==> Absolute Lower Address   | ------------ ==> Absolute Lower Address
```


2. Kernel Stack and Trap Frame
- The kernel stack is primarily used to store values for the calls performed by the kernel when an `ecall` or exception occurs in the kernel
- Each user process is given its own kernel stack aside from it own user stack for functions calls within the user process code.
- The Trapframe is an essential part of the kernel stack which is stored in the kernel stack but this must not be touched.
- The main function of the trapframe is to save the state of the process when an `ecall` or exception occurs and the Mode is changed from U-Mode to S-Mode
- The trapframe stores all the 32 registers and essential registers such as global pointer, stack pointer and thread pointer etc
- The value must be pushed into the trapframe before the exception or `ecall` is handled by the handler
- When an exception or `ecall` is handled by the kernel and the kernel performs an `sret`, Before the `sret` execution the values from the trapframe must be popped properly into the respective registers, This make sure that we would save the process state

Given below is the Diagram of how a Kernel Stack and Trapframe is structures
```
Allocate a 4KB Page for the kernel stack and trapframe
The stack grows downwards into lower memory addresses
--------- ==> Allocated Addr + 4096 bytes


--------- ==> Allocated Addr + 4096 bytes - sizeof(trapframe_t)





--------- ==> Allocated Addr

Therefore we must set the stack pointer to (Allocated Addr + 4096bytes - sizeof(trapframe_t)
before popping the values of the trapframe_t
```

- The Crucial thing to note here before popping of the trapframe_t is that the sp must be popped as the last element before performing `sret`.
- Since the sp is required to perform the popping process itself

Give below is an example of the trapframe_t struct, containing essential registers to be stored

```c

typedef struct trapframe_t{
  uint64_t temp_reg[7];
  uint64_t func_args[8];
  uint64_t saved_reg[12];
  uint64_t ret_addr;
  uint64_t s_stat;
  uint64_t except_pc;
  uint64_t thread_pointer;
  uint64_t global_pointer;
  uint64_t stack_pointer;
}__attribute__((packed)) trapframe_t;
```

3. The Proc Struct
- The Proc is required for two main reasons
i. The Struct is used to switch out processes when an timer interrupt occurs tracking the struct is required so we can have priority and scheduler work properly
ii. The struct has essential components to every process like the new page tables for that particular process and its trapframe_t also its pid.

Here is the example of such a struct
```c
typedef struct Proc{
  uint8_t pid;
  void* root_page_table;
  trapframe_t* tf;
}__attribute__((packed)) Proc;
```



