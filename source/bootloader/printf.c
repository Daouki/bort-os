#include "printf.h"

#include <stdarg.h>

#define CLEAR_STRING(String, Length, Value) \
        do { for (uint32_t i = 0; i < Length; i += 1) String[i] = Value; } while (0)

#define FLUSH_STRING(SystemTable, String, StringLength) \
        SystemTable->ConOut->OutputString(SystemTable->ConOut, String); StringLength = 0;

#define STRING_LENGTH 64

/// @return Length of the string.
static EFIAPI uint32_t integer_to_string(wchar_t* const string, int number, const int base) {
    static const wchar_t digits[] = L"0123456789ABCDEFGHIJKLMNOPQRSTWUVXYZ";

    if (base < 2 || base > 32) {
        return 0;
    }

    uint32_t string_position = 0;
    if (number < 0) {
        string[string_position++] = '-';
        number = -number;
    }

    uint32_t number_length = 0;
    {
        int number_copy = number;
        while (number_copy != 0) {
            number_copy /= base;
            number_length += 1;
        }
    }

    for (uint32_t i = 0; number != 0; i += 1) {
        string[string_position + number_length - i - 1] = digits[number % base];
        number /= base;
    }

    return string_position + number_length;
}

EFIAPI void printf(efi_system_table* system_table, const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);

    wchar_t string[65];
    uint32_t string_length = 0;
    CLEAR_STRING(string, STRING_LENGTH + 1, '\0');

    for (; *format != '\0'; format += 1) {
        char c = *format;
        if (c != '%') {
            string[string_length++] = c;
            if (string_length >= STRING_LENGTH) {
                FLUSH_STRING(system_table, string, string_length);
                CLEAR_STRING(string, STRING_LENGTH, '\0');
            }
        } else {
            if (string_length > 0) {
                FLUSH_STRING(system_table, string, string_length);
                CLEAR_STRING(string, STRING_LENGTH, '\0');
            }

            format += 1;
            c = *format;
            switch (c) {
                case 'd':
                case 'i': {
                    string_length = integer_to_string(string, va_arg(args, int), 10);
                    break;
                }

                default: {
                    string[string_length++] = c;
                }
            }
        }
    }

    if (string_length > 0) {
        FLUSH_STRING(system_table, string, string_length);
    }

    va_end(args);
}
