#pragma once

#include <efi/system-table.h>

EFIAPI void printf(efi_system_table* system_table, const wchar_t* format, ...);
