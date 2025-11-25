#pragma once
#include "exceptions.h"
#include "basicIO.h"

class InputValidator {
public:
    // 🔑 Only 2 parameters, no unused variables
    template<typename T>
    static T validateInt(T min_val, T max_val) {
        char buffer[32] = {0};
        
        long bytes = syscall3(0, 0, (long)buffer, 31);
        if (bytes <= 0) throw InvalidInputException();
        buffer[bytes] = '\0';
        
        // Validate it's a number
        int i = 0;
        bool neg = false;
        if (buffer[0] == '-') {
            neg = true;
            i = 1;
            if (i >= bytes) throw InvalidInputException();
        }
        
        for (; i < bytes && buffer[i] != '\n'; ++i) {
            if (buffer[i] < '0' || buffer[i] > '9') {
                throw InvalidInputException();
            }
        }
        
        T result = 0;
        i = neg ? 1 : 0;
        for (; i < bytes && buffer[i] != '\n'; ++i) {
            result = result * 10 + (buffer[i] - '0');
        }
        if (neg) result = -result;
        
        if (result < min_val || result > max_val) {
            throw OutOfRangeException();
        }
        
        return result;
    }
    
    static void validateSpectrum(double assigned, double max_allowed) {
        if (assigned > max_allowed) {
            throw InvalidSpectrumException();
        }
    }
};
