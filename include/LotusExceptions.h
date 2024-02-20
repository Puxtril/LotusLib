#pragma once

#include <exception>
#include <stdexcept>
#include <sstream>
#include <string>

namespace LotusLib
{
    class LotusException : public std::runtime_error {
    public:
        LotusException(const std::string& msg);
    };

    class InvalidDataException : public LotusException {
    public:
        InvalidDataException(const std::string& msg);
    };

    class DecompressionException : public LotusException {
    public:
        DecompressionException(const std::string& msg);
    };

    class InvalidLotusPath : public LotusException {
    public:
        InvalidLotusPath(const std::string& msg);
    };
}
