#pragma once

#ifdef DEBUG_MODE

#define EXECUTE_IF_DEBUG(x)		x
#define DEBUG_PRINT(msg)		std::cout << msg
#define DEBUG_PRINT_ERROR(msg)	std::cerr << msg

#else

#define EXECUTE_IF_DEBUG(x)		(void)0
#define DEBUG_PRINT(msg)		(void)0
#define DEBUG_PRINT_ERROR(msg)  (void)0

#endif