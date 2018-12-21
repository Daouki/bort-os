#include <efi/protocol/graphics-output.h>
#include <efi/boot-services.h>
#include <efi/runtime-services.h>
#include <efi/system-table.h>
#include <efi/types.h>

#include "printf.h"

#define _UNUSED_ __attribute__((unused))

#define EFI_PRINT(SystemTable, String) \
        SystemTable->ConOut->OutputString(system_table->ConOut, L##String)

#define EFI_PRINTLN(SystemTable, String) \
        SystemTable->ConOut->OutputString(system_table->ConOut, L##String "\r\n")

#define EFI_CHECK(EfiStatus, Expression) \
        EfiStatus = Expression; if (EFI_ERROR(EfiStatus)) return efi_status

#define GLOBAL_BUFFER_SIZE 32768U
static uint8_t g_buffer[GLOBAL_BUFFER_SIZE];

typedef struct GraphicsModeInfo {
    uint32_t  width;
    uint32_t  height;
    uint32_t  pitch;
    uint32_t* framebuffer;
} GraphicsModeInfo;

EFIAPI size_t get_memory_map_key(efi_system_table* system_table, efi_status* efi_status) {
    size_t memory_map_key, descriptor_size, buffer_size = GLOBAL_BUFFER_SIZE;
    uint32_t descriptor_version;
    *efi_status = system_table->BootServices->GetMemoryMap(
            &buffer_size, (efi_memory_descriptor*)g_buffer, &memory_map_key, &descriptor_size, 
            &descriptor_version);
    return memory_map_key;
}

EFIAPI efi_status efi_main(efi_handle efi_image _UNUSED_, efi_system_table* system_table) {
    efi_status efi_status;

    efi_status = system_table->ConOut->ClearScreen(system_table->ConOut);
    efi_status = EFI_PRINTLN(system_table, "BortOS Bootloader");
    if (EFI_ERROR(efi_status)) {
        return efi_status;
    }

    // Reset the watchdog timer.
    efi_status = system_table->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
    if (EFI_ERROR(efi_status)) {
        EFI_PRINTLN(system_table, "Error: Failed to reset the watchdog timer.");
        return efi_status;
    }

    size_t memory_map_key = get_memory_map_key(system_table, &efi_status);
    if (EFI_ERROR(efi_status)) {
        EFI_PRINTLN(system_table, "Error: Failed to obtain the memory map key.");
        return efi_status;
    }

    efi_status = system_table->BootServices->ExitBootServices(efi_image, memory_map_key);
    if (EFI_ERROR(efi_status)) {
            EFI_PRINTLN(system_table, "Error: Failed to exit EFI boot services.");
            printf(system_table, L"Error: Failed to exit EFI boot services, reason: %d.\r\n", efi_status);
            return efi_status;
    }

    while (true) {}

    return EFI_SUCCESS;
}
