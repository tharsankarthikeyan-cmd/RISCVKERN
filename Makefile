# --- Toolchain Configurations ---
CC      := riscv64-elf-gcc
AS      := riscv64-elf-as
LD      := riscv64-elf-ld
OBJCOPY := riscv64-elf-objcopy

# --- Compilation and Linking Flags ---
CFLAGS  := -Wall -Wextra -g -O0 -c -mcmodel=medany -ffreestanding -march=rv64gc_zbb -Iinclude
ASFLAGS := -g -c
LDFLAGS := -T linker.ld -nostdlib

# --- Target Definitions ---
TARGET_ELF := build/kernel.elf
TARGET_BIN := build/kernel.bin

# --- Source and Object File Discovery ---
# Looks inside the new src/ directory for your files
C_SRCS   := $(wildcard src/*.c)
ASM_SRCS := $(wildcard src/*.S)

# Map source files to their object file targets inside build/
OBJS     := $(patsubst src/%.c, build/%.o, $(C_SRCS)) \
            $(patsubst src/%.S, build/%.o, $(ASM_SRCS))

# --- Build Rules ---

# Default rule
all: $(TARGET_ELF) $(TARGET_BIN)

# Link rule: combines all object files into the final ELF executable
$(TARGET_ELF): $(OBJS) linker.ld
	@mkdir -p build
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGET_ELF)

# Pattern rule for compiling C source files into Object files
build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) $< -o $@

# Pattern rule for assembling .S assembly files into Object files
build/%.o: src/%.S
	@mkdir -p build
	$(AS) $(ASFLAGS) $< -o $@

# Extracts a raw binary file from the ELF
$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $(TARGET_ELF) $(TARGET_BIN)

# Clean rule: removes the entire build directory
clean:
	rm -rf build

.PHONY: all clean
