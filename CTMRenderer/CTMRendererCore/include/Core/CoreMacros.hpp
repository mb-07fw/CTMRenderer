#pragma once

#ifdef DEBUG_MODE

#include <string>
#include <iostream>

#define DO_WRAP(x)              do { x } while(0)
#define IF_DEBUG(x)				x
#define DEBUG_PRINT(msg)		std::cout << "[DEBUG_PRINT] " << msg 
#define DEBUG_PRINTW(msg)		std::wcout << L"[DEBUG_PRINTW] " << msg
#define DEBUG_PRINT_ERROR(msg)	std::cerr << "[DEBUG_PRINT_ERROR] " << msg
#define DEBUG_PRINT_ERRORW(msg)	std::wcerr << L"[DEBUG_PRINT_ERRORW] " << msg
#define DEBUG_ERROR(msg, code)	DO_WRAP( \
									DEBUG_PRINT_ERROR(msg); \
									__debugbreak();\
									exit(code); \
								)
#define RUNTIME_ASSERT(x, msg)	if (!(x)) \
								{ \
									std::cerr << "[RUNTIME_ASSERT] Assert failed. \n"; \
									std::cerr << "[RUNTIME_ASSERT] Condition : " << #x << '\n'; \
									std::cerr << "[RUNTIME_ASSERT] File : " << __FILE__ << '\n'; \
									std::cerr << "[RUNTIME_ASSERT] Line : " << __LINE__ << '\n'; \
									DEBUG_ERROR(msg, 1); \
								} \

#else

#define DO_WRAP(x)				(void)0
#define IF_DEBUG(x)				(void)0
#define DEBUG_PRINT(msg)		(void)0
#define DEBUG_PRINT_ERROR(msg)  (void)0
#define DEBUG_ERROR(msg, code)  (void)0
#define RUNTIME_ASSERT(x, msg)  (void)0

#endif