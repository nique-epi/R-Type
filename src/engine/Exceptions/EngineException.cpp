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

EmptyEventCallbackException::EmptyEventCallbackException()
    : EngineException("An empty callback cannot subscribe to events") {}

NetworkIdExhaustedException::NetworkIdExhaustedException()
    : EngineException("Every network identifier was already handed out") {}

UnknownLaunchArgumentException::UnknownLaunchArgumentException(
    const std::string& argument)
    : EngineException("Unknown argument \"" + argument + "\"") {}

MissingLaunchOptionValueException::MissingLaunchOptionValueException(
    const std::string& option)
    : EngineException("Missing value after the option " + option) {}

InvalidLaunchOptionValueException::InvalidLaunchOptionValueException(
    const std::string& option, const std::string& value)
    : EngineException("Invalid value \"" + value + "\" after the option " +
                      option) {}

NullModuleException::NullModuleException()
    : EngineException("A null module cannot be given to an application") {}

MissingDependencyException::MissingDependencyException(
    const std::string& moduleName, const std::string& dependency)
    : EngineException("The module \"" + moduleName +
                      "\" depends on the module \"" + dependency +
                      "\", which is not listed before it") {}

ModuleStartFailedException::ModuleStartFailedException(
    const std::string& moduleName, const std::string& reason)
    : EngineException("The module \"" + moduleName +
                      "\" failed to start: " + reason) {}

UnknownStageException::UnknownStageException(const std::string& stageName)
    : EngineException("The loop has no stage named \"" + stageName + "\"") {}

DuplicateServiceException::DuplicateServiceException(
    const std::string& moduleName, const std::string& serviceName)
    : EngineException("The module \"" + moduleName +
                      "\" provides the service " + serviceName +
                      ", which is already provided") {}

MissingServiceException::MissingServiceException(const std::string& moduleName,
                                                 const std::string& serviceName)
    : EngineException("The module \"" + moduleName +
                      "\" requires the service " + serviceName +
                      ", which no module installed before it provides") {}

}  // namespace rtype::engine
