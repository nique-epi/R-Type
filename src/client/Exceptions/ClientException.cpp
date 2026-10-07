#include "ClientException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::client {

ClientException::ClientException(const std::string& message)
    : std::runtime_error(message) {}

FontNotLoadedException::FontNotLoadedException(const std::string& file)
    : ClientException("The font file could not be loaded: " + file) {}

RenderTextureNotCreatedException::RenderTextureNotCreatedException(
    unsigned int width, unsigned int height)
    : ClientException("A render texture could not be created, size " +
                      std::to_string(width) + " x " + std::to_string(height)) {}

}  // namespace rtype::client
