#pragma once

#ifdef _DEBUG
#define print_message(prefix, ...) DbgPrintEx(0, 0, "[" prefix "] " __VA_ARGS__)
#else
#define print_message(prefix, ...) 
#endif

#define print_success_message(...) print_message("+", __VA_ARGS__)
#define print_error_message(...) print_message("-", __VA_ARGS__)
