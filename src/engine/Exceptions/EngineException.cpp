#include "EngineException.hpp"
#include <stdexcept>
#include <string>

namespace rtype::engine {

EngineException::EngineException(const std::string& message)
    : std::runtime_error(message) {}

InvalidTickDurationException::InvalidTickDurationException()
    : EngineException("The tick duration must be strictly positive") {}

InvalidTimerDelayException::InvalidTimerDelayException()
    : EngineException("The timer delay must not be negative") {}

InvalidTimerIntervalException::InvalidTimerIntervalException()
    : EngineException("The timer interval must be strictly positive") {}

NegativeElapsedTimeException::NegativeElapsedTimeException()
    : EngineException("The elapsed simulation time must not be negative") {}

}  // namespace rtype::engine
