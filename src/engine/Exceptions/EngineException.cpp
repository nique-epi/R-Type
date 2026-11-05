#include "EngineException.hpp"
#include <stdexcept>
#include <string>
#include <vector>

namespace rtype::engine {

namespace {

std::string joinedFolders(const std::vector<std::string>& folders) {
  std::string joined;
  for (const std::string& folder : folders) {
    if (!joined.empty()) {
      joined += ", ";
    }
    joined += folder;
  }
  return joined;
}

}  // namespace

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

DuplicateCommandException::DuplicateCommandException(
    const std::string& commandName)
    : EngineException("The command \"" + commandName +
                      "\" is already registered") {}

DuplicateSettingException::DuplicateSettingException(
    const std::string& settingName)
    : EngineException("The setting \"" + settingName +
                      "\" is already declared") {}

CommandRefusedException::CommandRefusedException(const std::string& reason)
    : EngineException(reason) {}

UnknownExecutablePathException::UnknownExecutablePathException()
    : EngineException(
          "The system did not tell where the running executable is, so the "
          "files next to it cannot be found") {}

AssetFolderNotFoundException::AssetFolderNotFoundException(
    const std::vector<std::string>& searchedFolders)
    : EngineException("No assets folder found, searched: " +
                      joinedFolders(searchedFolders)) {}

SharedLibraryOpenException::SharedLibraryOpenException(
    const std::string& file, const std::string& reason)
    : EngineException("The library \"" + file +
                      "\" could not be opened: " + reason) {}

}  // namespace rtype::engine
