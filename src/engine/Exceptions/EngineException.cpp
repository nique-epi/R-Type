#include "EngineException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::engine {

EngineException::EngineException(const std::string& message)
    : std::runtime_error(message) {}

InvalidTickDurationException::InvalidTickDurationException()
    : EngineException("The tick duration must be strictly positive") {}

}  // namespace rtype::engine
