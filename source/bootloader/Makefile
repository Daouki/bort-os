MODULE_DIR := $(patsubst %/,%,$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))

SOURCE_FILES := $(shell find $(MODULE_DIR) -name '*.c' -type f)

OBJECT_FILES := $(patsubst $(CURRENT_DIR)/%,$(BUILD_DIR)/bootloader/%,$(SOURCE_FILES))
OBJECT_FILES := $(addsuffix .o,$(OBJECT_FILES))

# -fpic ???
CC_FLAGS += -target x86_64-pc-win32-coff -I$(LIBRARIES_DIR)/zircon-efi
LD_FLAGS += -subsystem:efi_application -dll

all: $(OBJECT_FILES)
	@$(LD) $(LD_FLAGS) -entry:efi_main -out:$(BUILD_DIR)/bootx64.efi $(OBJECT_FILES)
	@mkdir -p $(BUILD_DIR)/efi/boot
	@cp $(BUILD_DIR)/bootx64.efi $(BUILD_DIR)/efi/boot

$(BUILD_DIR)/bootloader/%.c.o: $(CURRENT_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CC_FLAGS) -c -o $@ $<
