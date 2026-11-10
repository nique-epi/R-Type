#include "EngineException.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "DataFilePlace.hpp"
#include "ErrorText.hpp"
#include "WholeNumberRange.hpp"

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
                      listText(searchedFolders)) {}

UnreadableDataFileException::UnreadableDataFileException(
    const std::string& file)
    : EngineException("cannot read the file " + file) {}

DataFileSyntaxException::DataFileSyntaxException(const DataFilePlace& place,
                                                 const std::string& description)
    : EngineException(placeText(place) + ": " + description) {}

MissingDataFileKeyException::MissingDataFileKeyException(
    const DataFilePlace& place)
    : EngineException(placeText(place) + ": " + place.key + " is missing") {}

UnknownDataFileKeyException::UnknownDataFileKeyException(
    const DataFilePlace& place)
    : EngineException(placeText(place) + ": unknown key " + place.key) {}

DataFileTableExpectedException::DataFileTableExpectedException(
    const DataFilePlace& place)
    : EngineException(placeText(place) + ": " + place.key +
                      " must be a table") {}

DataFileTablesExpectedException::DataFileTablesExpectedException(
    const DataFilePlace& place)
    : EngineException(placeText(place) + ": " + place.key +
                      " must be an array of tables") {}

DataFileValueOutOfRangeException::DataFileValueOutOfRangeException(
    const DataFilePlace& place, WholeNumberRange range)
    : EngineException(placeText(place) + ": " + place.key +
                      " must be an integer from " + rangeText(range)) {}

DataFileTextExpectedException::DataFileTextExpectedException(
    const DataFilePlace& place)
    : EngineException(placeText(place) + ": " + place.key + " must be a text") {
}

UnknownDataFileChoiceException::UnknownDataFileChoiceException(
    const DataFilePlace& place, const std::string& value,
    std::span<const std::string_view> choices)
    : EngineException(placeText(place) + ": " + place.key +
                      ": unknown value \"" + value +
                      "\"; known: " + listText(choices)) {}

SharedLibraryOpenException::SharedLibraryOpenException(
    const std::string& file, const std::string& reason)
    : EngineException("The library \"" + file +
                      "\" could not be opened: " + reason) {}

PluginNotFoundException::PluginNotFoundException(const std::string& name,
                                                 const std::string& file)
    : EngineException("The plugin \"" + name + "\" is not in " + file) {}

PluginEntryNotFoundException::PluginEntryNotFoundException(
    const std::string& name, const std::string& entryName)
    : EngineException("The plugin \"" + name + "\" exports no " + entryName) {}

PluginApiVersionException::PluginApiVersionException(
    const std::string& name, std::uint32_t pluginVersion,
    std::uint32_t programVersion)
    : EngineException("The plugin \"" + name + "\" was built for plugin API " +
                      std::to_string(pluginVersion) +
                      "; this program loads plugin API " +
                      std::to_string(programVersion)) {}

InvalidPluginNameException::InvalidPluginNameException(const std::string& name)
    : EngineException("\"" + name +
                      "\" is not a plugin name: letters, digits, '-' and '_' "
                      "only") {}

DuplicateModuleNameException::DuplicateModuleNameException(
    const std::string& name)
    : EngineException("Two modules are named \"" + name + "\"") {}

InvalidEnsureLineException::InvalidEnsureLineException(const std::string& line)
    : EngineException("\"" + line + "\" must be: ensure <resource name>") {}

InvalidResourceNameException::InvalidResourceNameException(
    const std::string& name, std::size_t longestName)
    : EngineException("\"" + name + "\" is not a resource name: 1 to " +
                      std::to_string(longestName) +
                      " lowercase letters, digits, '-' and '_'") {}

ResourceNotFoundException::ResourceNotFoundException(const std::string& name,
                                                     const std::string& folder)
    : EngineException("The resource \"" + name + "\" has no manifest in " +
                      folder) {}

ResourceDependencyCycleException::ResourceDependencyCycleException(
    const std::vector<std::string>& chain)
    : EngineException("The resources depend on each other in a circle: " +
                      listText(chain)) {}

ResourceDependenciesExpectedException::ResourceDependenciesExpectedException(
    const DataFilePlace& place)
    : EngineException(placeText(place) + ": " + place.key +
                      " must be a list of resource names") {}

ScriptException::ScriptException(const std::string& message)
    : EngineException(message) {}

InvalidGridLayoutException::InvalidGridLayoutException(std::size_t maximumLines)
    : EngineException(
          "A collision grid needs a positive cell side and at most " +
          std::to_string(maximumLines) + " cells along each axis") {}

}  // namespace rtype::engine
