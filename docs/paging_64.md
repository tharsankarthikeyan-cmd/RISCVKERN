# Paging in 64 Bit Systems and Fundamental Difference between 32 bit Paging
## Understanding Paging in 32 Bit Systems
- Paging in 32 bit systems is based on factor called 1024 Multiplier
- Since the total available memory is 2^32 bytes or 4GB
- The system uses 32 bit addresses and the Paging Virtual Address are also 32 Bit
- Assume an Address 0x00000000
- 4KB Pages are there meaning, 4096 Bytes needs to be accommodated
- 4*1024 = 2^2 * 2^10 = 2^12 = 4096
- Therefore we must use the first 12 bits(LSB) for the page offset
- for 4MB Pages, there are exactly 1024*4KB pages meaning 2^10 4KB
- Therefore The Next 10 Bits are used for Page Table Entries which select a Page.
- There are exactly 1024*4MB in 4GB, Meaning 2^10*4MB
- Therefore The next bits are allotted for the Page Directory Entries which select a Page Table Entry
- In Summary
```
| 10 Bits(PDE Offset) | 10 Bits(PTE Offset) | 12 Bits(Page Offset) |
31                    21                    11                     0

4GB ==> 1024 * 4MB
4MB ==> 1024 * 4KB
Each 4KB is a single Page

```
- This Matched up directly along with the 32 Bit Address
- The Virtual and Physical addresses are same in an Identity Mapping

## Understanding Paging 64 bit Systems
- Paging in 64 bit system is a bit more complicated than the 32 bits systems
- Here we have 2^64 bytes of memory possible which is equivalent to 16 Exabytes
- Memory such big is not even used in supercomputer meaning...
- We shall not use the entire 64 bits of address for Paging
- Hence we introduce the Sv39 paging ,Sv48 Paging and, Sv57 Paging
- NOTE: 39, 48 and 57 are the respective length of the Virtual Address
- In all the 64 bit systems, We never use 64 bit addresses we only use 56 bit addresses which is more preffered
- Let's now see how the Virtual and Physical Address correspond with each other
> [!IMPORTANT]
> It is very important to notes that inn 64 bits system we use the so called 512 Multiplier system instead of 1024 Multiplier

- Let's begin with page offsets: A page must be 4KB meaning, 4096 bytes, Hence similar to the 32 bit system we take LSB 12 bits for Page Offset
- 512 * 4KB pages make us a Page Table (Level 0) which gives us, 2^9*4Kb pages leading us to 2MB
- IMPORTANT to notes that the Page Table entries of the Level 0 now correspond to 2MB Huge Pages consists of 512 4KB leafs per 2MB Huge Page
- Also for Level 0 we allot 9 bits (2^9 = 512)
- Similarly we also allot 9 bits for the next Page Table (Level 1) which consists of 512 * 2MB = 2^9 * 2MB = 1GB
- Page Table Entries of Level 1 Now correspond to 1GB Mega Pages consisting of 512, 2MB Huge Pages
- Furthermore we again allot the next 9 bits towards Page Table (Level 2) which consists of 512 * 1GB = 512 GB.
- Now almost the entire stuff is Covered
- This is the Sv39, Three Level Paging Protocol
- We have now accommodated for the 9+9+9+12 = 39 bits of the Virtual Address
- How is the Physical Address Structured
- Again we follow the same logic
- 12 bits for Page Offset, 9 bits for PGT(Level 0), 9 bits for PGT(Level 1), 9 bits for PGT(Level 2) = 39 bits
- The remaining for the bits which is 56 - 39 = 26 are still present just not used yet.
- For Bigger Paging Schemes such are Sv48 and Sv57 we would use them for larger RAM and memory access
- In Summary
```
According to the Documentation

VPN(Virutal Page Number is the Virtual Address)
| VPN[2] | VPN[1] | VPN[0] | Page Offset |
39       30       21       12            0

PPN(Physical Page Number is the Physical Address)
| PPN[2] Extended | PPN[2] | PPN[1] | PPN[0] | Page Offset |
56       39      30       21       12             0
```


- Given above is the correct structure of paging



