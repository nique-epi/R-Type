#pragma once

#include <stdexcept>
#include <string>

namespace rtype::engine {

/**
 * @brief Root of every error raised by the engine library.
 */
class EngineException : public std::runtime_error {
 public:
  explicit EngineException(const std::string& message);
};

/**
 * @brief A fixed time step was requested with a duration that is not positive.
 */
class InvalidTickDurationException : public EngineException {
 public:
  InvalidTickDurationException();
};

}  // namespace rtype::engine
