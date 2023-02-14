#pragma once

#ifdef _DEBUG
#define DEBUG_PRINT(prefix, ...) DbgPrintEx(0, 0, "[" prefix "] " __VA_ARGS__)
#else
#define DEBUG_PRINT(prefix, ...) 
#endif

#define SUCCESS_PRINT(...) DEBUG_PRINT("+", __VA_ARGS__)
#define ERROR_PRINT(...) DEBUG_PRINT("-", __VA_ARGS__)
