RPI_VERSION ?= 4

BOOTMNT ?= /workspace/build/boot

# Target architecture and compiler settings
ARCH ?= aarch64

# ----- toolchain selection (portable across Linux/macOS) -----
TARGET 		= aarch64-none-elf
CC          ?= clang
AS          ?= clang

# Try to find ld.lld and llvm-objcopy on PATH (Linux, macOS Homebrew, etc.)
# Check common locations for macOS Homebrew and Linux package managers
LD          ?= $(shell command -v ld.lld 2>/dev/null || echo ld.lld)
OBJCOPY     ?= $(shell command -v llvm-objcopy 2>/dev/null || command -v /opt/homebrew/opt/llvm/bin/llvm-objcopy 2>/dev/null || echo llvm-objcopy)

# If ld.lld isn't on PATH but clang is, fall back to linking via clang with -fuse-ld=lld
USE_CLANG_LINK ?= $(shell if command -v $(LD) >/dev/null 2>&1; then echo 0; else echo 1; fi)

COMMON_FLAGS = --target=$(TARGET) -mcpu=cortex-a72 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector

COPS = $(COMMON_FLAGS) -Wall -mgeneral-regs-only -Iinclude -DRPI_VERSION=$(RPI_VERSION)

COPS += -DQEMU_TESTING


BUILD_DIR = build
SRC_DIR = src

# Define linker script location
LINKER_SCRIPT = src/linker.ld

all: $(BUILD_DIR)/kernel8.img

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(COPS) -MMD -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(CC) $(COMMON_FLAGS) -c $< -o $@

$(BUILD_DIR)/%_S.o: $(SRC_DIR)/%.S
	mkdir -p $(dir $@)
	$(CC) $(COPS) -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_S_FILES = $(wildcard $(SRC_DIR)/*.s)
ASM_S_CAP_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%_c.o, $(C_FILES))
OBJ_FILES += $(patsubst $(SRC_DIR)/%.s, $(BUILD_DIR)/%_s.o, $(ASM_S_FILES))
OBJ_FILES += $(patsubst $(SRC_DIR)/%.S, $(BUILD_DIR)/%_S.o, $(ASM_S_CAP_FILES))

DEP_FILES = $(OBJ_FILES:.o=.d)
-include $(DEP_FILES)

$(BUILD_DIR)/kernel8.img: $(OBJ_FILES) $(LINKER_SCRIPT) | $(BUILD_DIR)
	@echo "Building for RPI $(value RPI_VERSION)"
	@echo "Deploy to $(value BOOTMNT)"
	@echo ""
	mkdir -p $(BOOTMNT)
ifeq ($(USE_CLANG_LINK),0)
	@echo "Using direct linker: $(LD)"
	$(LD) -T $(LINKER_SCRIPT) -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
else
	@echo "Using clang with lld linker"
	$(CC) $(COMMON_FLAGS) -fuse-ld=lld -Wl,-T,$(LINKER_SCRIPT) -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
endif
	$(OBJCOPY) $(BUILD_DIR)/kernel8.elf -O binary $@
ifeq ($(RPI_VERSION), 4)
	cp $(BUILD_DIR)/kernel8.img $(BOOTMNT)/kernel8-rpi4.img
else
	cp $(BUILD_DIR)/kernel8.img $(BOOTMNT)/
endif
	cp config.txt $(BOOTMNT)/
	sync

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: all clean

