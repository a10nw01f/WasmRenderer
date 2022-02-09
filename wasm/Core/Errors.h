#include <iostream>

inline void FatalError(const char* err)
{
    std::cout << "Fatal Error:" << err << "\n";
    exit(-1);
}