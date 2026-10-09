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

/**
 * @brief A timer was scheduled with a negative delay.
 */
class InvalidTimerDelayException : public EngineException {
 public:
  InvalidTimerDelayException();
};

/**
 * @brief A repeating timer was scheduled with an interval that is not
 * positive.
 */
class InvalidTimerIntervalException : public EngineException {
 public:
  InvalidTimerIntervalException();
};

/**
 * @brief Simulation time was advanced by a negative amount.
 */
class NegativeElapsedTimeException : public EngineException {
 public:
  NegativeElapsedTimeException();
};

/**
 * @brief A component was added to an entity that is not alive.
 */
class DeadEntityException : public EngineException {
 public:
  DeadEntityException();
};

/**
 * @brief A component was added or removed while a query was visiting
 * components.
 */
class ComponentChangeDuringIterationException : public EngineException {
 public:
  ComponentChangeDuringIterationException();
};

/**
 * @brief A null system was added to the scheduler.
 */
class NullSystemException : public EngineException {
 public:
  NullSystemException();
};

/**
 * @brief An event subscription was made with an empty callback.
 */
class EmptyEventCallbackException : public EngineException {
 public:
  EmptyEventCallbackException();
};

}  // namespace rtype::engine
