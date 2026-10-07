#include "ClientException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::client {

ClientException::ClientException(const std::string& message)
    : std::runtime_error(message) {}

FontNotLoadedException::FontNotLoadedException(const std::string& file)
    : ClientException("The font file could not be loaded: " + file) {}

}  // namespace rtype::client
