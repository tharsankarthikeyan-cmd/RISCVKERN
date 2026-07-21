void _start() {
    // Pointer to the volatile UART memory address
    volatile char *uart = (volatile char *)0x10000000;
    
    while(1) {
        *uart = '*'; // Write normally to the memory address
    }
}
