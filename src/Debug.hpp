// Debug.hpp
#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>

// 定义 DEBUG 宏以启用调试输出
// #define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(x) std::cout << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

#endif // DEBUG_HPP
