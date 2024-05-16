#pragma once

#include <stdexcept>
#include <string>

namespace flang
{


// ----- Control Flow Exceptions -----

class flang_return : public std::runtime_error
{
public:
    explicit flang_return()
        : std::runtime_error("")
    {
    }
};

class flang_break : public std::runtime_error
{
public:
    explicit flang_break()
        : std::runtime_error("")
    {
    }
};

// ----- Exceptions that indicate error -----

class flang_exception : public std::runtime_error
{
public:
    explicit flang_exception(std::string const& e_msg)
        : std::runtime_error(e_msg)
    {
    }
};


class flang_runtime_error : public flang_exception
{
public:
    explicit flang_runtime_error(std::string const& e_msg)
        : flang_exception(e_msg)
    {
    }
};

class tokenizer_exception : public flang_exception
{
public:
    explicit tokenizer_exception(std::string const& e_msg)
        : flang_exception(e_msg)
    {
    }
};

class parser_exception : public flang_exception
{
public:
    explicit parser_exception(std::string const& e_msg)
        : flang_exception(e_msg)
    {
    }
};
} // namespace flang
