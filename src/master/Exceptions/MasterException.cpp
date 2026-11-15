#include "MasterException.hpp"
#include <stdexcept>
#include <string>
#include <string_view>

namespace rtype::master {

MasterException::MasterException(const std::string& message)
    : std::runtime_error(message) {}

DatabaseClosedException::DatabaseClosedException()
    : MasterException("the database of the master is not open") {}

InvalidPasswordKeyException::InvalidPasswordKeyException(
    std::string_view variable)
    : MasterException(std::string{variable} +
                      " holds no key: 64 hexadecimal digits are expected, as "
                      "`openssl rand -hex 32` draws them; leave it unset to "
                      "run without accounts") {}

}  // namespace rtype::master
