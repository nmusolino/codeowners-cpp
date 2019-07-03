#include <codeowners/codeowners.hpp>

namespace co {

    std::string message(const std::string& recipient)
    {
        const std::string greeting { "Hello" };
        return recipient.empty() ? greeting : greeting + ", " + recipient;
    }

} /* end namespace 'co' */
