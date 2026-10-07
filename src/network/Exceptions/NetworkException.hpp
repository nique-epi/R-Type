#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

namespace rtype::network {

/**
 * @brief Root of every error raised by the network module.
 */
class NetworkException : public std::runtime_error {
 public:
  explicit NetworkException(const std::string& message);
};

/**
 * @brief A read asked for more bytes than the buffer still holds.
 */
class BufferUnderflowException : public NetworkException {
 public:
  BufferUnderflowException(std::size_t requestedBytes,
                           std::size_t availableBytes);
};

/**
 * @brief A string is longer than the length prefix can announce.
 */
class StringTooLongException : public NetworkException {
 public:
  StringTooLongException(std::size_t length, std::size_t maximumLength);
};

}  // namespace rtype::network
