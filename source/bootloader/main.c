#include <efi/boot-services.h>
#include <efi/runtime-services.h>
#include <efi/system-table.h>
#include <efi/types.h>

#define _UNUSED_ __attribute__((unused))

#define EFI_CHECK(EfiStatus, FuncCall) if (EfiStatus = EFI_ERROR(FuncCall))

efi_status efi_main(efi_handle efi_image _UNUSED_, efi_system_table* system_table) {
    efi_status efi_status;

    efi_status = system_table->ConOut->ClearScreen(system_table->ConOut);
    efi_status = system_table->ConOut->OutputString(system_table->ConOut, L"BortOS Bootloader");
    if (EFI_ERROR(efi_status)) {
        return efi_status;
    }

    while (true) {}

    return EFI_SUCCESS;
}
