#include "ServerException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::server {

ServerException::ServerException(const std::string& message)
    : std::runtime_error(message) {}

UnknownLaunchArgumentException::UnknownLaunchArgumentException(
    const std::string& argument)
    : ServerException("Unknown argument \"" + argument + "\"") {}

MissingLaunchOptionValueException::MissingLaunchOptionValueException(
    const std::string& option)
    : ServerException("Missing value after the option " + option) {}

}  // namespace rtype::server
