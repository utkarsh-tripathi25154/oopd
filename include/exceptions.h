#pragma once

// Base exception class
class CellularException {
protected:
    char message[128];
public:
    CellularException(const char* msg) {
        int i = 0;
        while (msg[i] && i < 127) { message[i] = msg[i]; ++i; }
        message[i] = '\0';
    }
    const char* what() const { return message; }
};

// Input validation exceptions
class InvalidInputException : public CellularException {
public:
    InvalidInputException() : CellularException("Invalid input: not a valid number") {}
};

class OutOfRangeException : public CellularException {
public:
    OutOfRangeException() : CellularException("Input out of valid range") {}
};

class InvalidSpectrumException : public CellularException {
public:
    InvalidSpectrumException() : CellularException("Spectrum exceeds hardware limits") {}
};
