#include <iostream>

#include "customizable.h"
#include "base.h"

void CopyMoveLoggerTests()
{
    // Sizeof CopyMoveLogger with pointer to std::ostream
    {

        std::cout << sizeof(BaseCopyMoveLogger{});
        std::cout << sizeof(char);
        struct C {} m;
        std::cout << sizeof(m);
    }

}
