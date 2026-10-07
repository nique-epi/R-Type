#include "NetworkException.hpp"
#include <cstddef>
#include <stdexcept>
#include <string>

namespace rtype::network {

NetworkException::NetworkException(const std::string& message)
    : std::runtime_error(message) {}

BufferUnderflowException::BufferUnderflowException(std::size_t requestedBytes,
                                                   std::size_t availableBytes)
    : NetworkException("Cannot read " + std::to_string(requestedBytes) +
                       " byte(s), only " + std::to_string(availableBytes) +
                       " left in the buffer") {}

StringTooLongException::StringTooLongException(std::size_t length,
                                               std::size_t maximumLength)
    : NetworkException("String of " + std::to_string(length) +
                       " byte(s) exceeds the maximum of " +
                       std::to_string(maximumLength)) {}

}  // namespace rtype::network
