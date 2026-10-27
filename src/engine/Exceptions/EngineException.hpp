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

/**
 * @brief Every network identifier was already handed out.
 */
class NetworkIdExhaustedException : public EngineException {
 public:
  NetworkIdExhaustedException();
};

/**
 * @brief An argument given at launch that is no option of the program.
 */
class UnknownLaunchArgumentException : public EngineException {
 public:
  explicit UnknownLaunchArgumentException(const std::string& argument);
};

/**
 * @brief A launch option that needs a value was the last argument.
 */
class MissingLaunchOptionValueException : public EngineException {
 public:
  explicit MissingLaunchOptionValueException(const std::string& option);
};

/**
 * @brief The value given after a launch option is not one the option accepts.
 */
class InvalidLaunchOptionValueException : public EngineException {
 public:
  InvalidLaunchOptionValueException(const std::string& option,
                                    const std::string& value);
};

/**
 * @brief A null module was given to an application.
 */
class NullModuleException : public EngineException {
 public:
  NullModuleException();
};

/**
 * @brief A module of an application requires a module that is not listed before
 * it.
 */
class MissingRequiredModuleException : public EngineException {
 public:
  MissingRequiredModuleException(const std::string& moduleName,
                                 const std::string& requiredModule);
};

/**
 * @brief A module of an application threw while it started.
 */
class ModuleStartFailedException : public EngineException {
 public:
  ModuleStartFailedException(const std::string& moduleName,
                             const std::string& reason);
};

}  // namespace rtype::engine
