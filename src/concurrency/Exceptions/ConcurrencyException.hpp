#pragma once

#include <stdexcept>
#include <string>

namespace rtype::concurrency {

/**
 * @brief Root of every error raised by the concurrency library.
 */
class ConcurrencyException : public std::runtime_error {
 public:
  explicit ConcurrencyException(const std::string& message);
};

/**
 * @brief A bounded queue was created with no room for any message.
 */
class InvalidQueueCapacityException : public ConcurrencyException {
 public:
  InvalidQueueCapacityException();
};

}  // namespace rtype::concurrency
