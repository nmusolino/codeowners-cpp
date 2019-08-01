#pragma once

#include <exception>
#include <string>

namespace co
{

class error : public std::exception
{
public:
    error(const std::string& message)
        : m_message { message }
    {
    }
    const char* what() const noexcept override { return m_message.c_str(); }

private:
    std::string m_message;
};

class repository_not_found_error : public error
{
public:
    using error::error;
};

}  // end namespace 'co'