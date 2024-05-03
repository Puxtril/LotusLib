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

    class DecompressionException : public LotusException {
    public:
        DecompressionException(const std::string& msg);
    };

    class CommonHeaderError : public LotusException {
    public:
        CommonHeaderError(const std::string& msg);
    };

    class InternalEntryNotFound : public LotusException {
    public:
        InternalEntryNotFound(const std::string& msg);
    };

    class InternalFileNotFound : public InternalEntryNotFound {
    public:
        InternalFileNotFound(const std::string& msg);
    };

    class InternalDirectoryNotFound : public InternalEntryNotFound {
    public:
        InternalDirectoryNotFound(const std::string& msg);
    };
}
