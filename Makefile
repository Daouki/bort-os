export ROOT_DIR      := $(patsubst %/,%,$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))
export BUILD_DIR     ?= $(ROOT_DIR)/build
export BINARY_DIR    := $(ROOT_DIR)/binary
export LIBRARIES_DIR := $(ROOT_DIR)/libraries
export SOURCE_DIR    := $(ROOT_DIR)/source

export IMAGE_PATH ?= $(BUILD_DIR)/bort-os.iso

export CC  := clang
export CXX := clang++
export LD  := lld-link

export QEMU := qemu-system-x86_64
export QEMU_FLAGS := -enable-kvm                                                                    \
                     -drive format=raw,file=$(BUILD_DIR)/bort-os.iso                                \
                     -drive if=pflash,format=raw,unit=0,file=$(BINARY_DIR)/OVMF_CODE.fd,readonly=on \
                     -drive if=pflash,format=raw,unit=1,file=$(BINARY_DIR)/OVMF_VARS.fd

export CC_FLAGS  := -std=c99 -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti -ffreestanding \
                    -flto -fno-stack-protector -fshort-wchar -mno-red-zone
export CXX_FLAGS := -std=c++17 -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti -ffreestanding \
                    -flto -fno-stack-protector -fshort-wchar -mno-red-zone
export LD_FLAGS  := -nodefaultlib

LIBRARIES := zircon-efi
MODULES   := bootloader

qemu: all
	$(QEMU) $(QEMU_FLAGS)

all: $(MODULES)
	@# Create the EFI partition image.
	@dd if=/dev/zero of=$(BUILD_DIR)/efi_partition.iso bs=1M count=64 2> /dev/null
	@mformat -i $(BUILD_DIR)/efi_partition.iso -F -h 32 -t 64 -s 64 -c 1
	@mmd -i $(BUILD_DIR)/efi_partition.iso ::/efi
	@mmd -i $(BUILD_DIR)/efi_partition.iso ::/efi/boot
	@mcopy -i $(BUILD_DIR)/efi_partition.iso $(BUILD_DIR)/bootx64.efi ::/efi/boot/bootx64.efi
	@# Create the disk image.
	@dd if=/dev/zero of=$(BUILD_DIR)/bort-os.iso bs=1M count=256 2> /dev/null
	@parted $(BUILD_DIR)/bort-os.iso -s -a minimal mklabel gpt
	@parted $(BUILD_DIR)/bort-os.iso -s -a minimal mkpart EFI FAT16 2048s 93716s
	@parted $(BUILD_DIR)/bort-os.iso -s -a minimal toggle 1 boot
	@# Copy the EFI partition image to the disk image.
	@dd if=$(BUILD_DIR)/efi_partition.iso of=$(BUILD_DIR)/bort-os.iso bs=512 count=131072 seek=2048 conv=notrunc 2> /dev/null

$(MODULES): $(LIBRARIES)
	@for module in $(MODULES); do          \
	    $(MAKE) -C $(SOURCE_DIR)/$$module; \
	done

$(LIBRARIES):
	@for library in $(LIBRARIES); do           \
	    $(MAKE) -C $(LIBRARIES_DIR)/$$library; \
	done

clean:
	@rm -rf $(BUILD_DIR)
