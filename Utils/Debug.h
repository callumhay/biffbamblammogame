#include <iostream>

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef  NDEBUG
#define debug_output(s) ((void)0)
#else
#define debug_output(s) std::cout << s << std::endl
#endif

#endif