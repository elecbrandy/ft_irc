#ifndef SCOPED_TIMER_HPP
#define SCOPED_TIMER_HPP

#include <ctime>
#include <iostream>
#include <string>
#include "Color.hpp"

class ScopedTimer {
public:
    ScopedTimer(const std::string& functionName)
        : funcName(functionName), start(clock()) {}

    ~ScopedTimer() {
        clock_t end = clock();
        double duration = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000.0;
        std::cout << C_WARN << "  TIMELOG: " << funcName << ": " << duration << "μs\n" << C_RESET;
    }

private:
    std::string funcName;
    clock_t start;
};

#endif
