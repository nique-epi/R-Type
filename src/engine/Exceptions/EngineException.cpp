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

DeadEntityException::DeadEntityException()
    : EngineException(
          "A component cannot be added to an entity that is not alive") {}

ComponentChangeDuringIterationException::
    ComponentChangeDuringIterationException()
    : EngineException(
          "Components cannot be added or removed while a query is visiting "
          "components") {}

NullSystemException::NullSystemException()
    : EngineException("A null system cannot be added to the scheduler") {}

}  // namespace rtype::engine
