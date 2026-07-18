# OpenSBI
- OpenSBI is analogically similar to PC BIOS, but it is work is quite different
- It manages all the hardware in the system and runs all the platform specific code
- It parses the Device Tree Blob which is used further by the operating system which runs in S-Mode
- OpenSBI hands over to the kernel control at 0x80200000 at kmain function
- Further mode, S-Mode cannot access all the MMIO, This must be done via ecall(A specific form of systemcall)
- ecall is a trap, which gets sent into the M-Mode Handler based on the argument register a0 to a7
- These a0 to a7 is used in order to form for S-Mode what it want to perform via ecalls


