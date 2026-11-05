#pragma once

#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include "DataFilePlace.hpp"
#include "WholeNumberRange.hpp"

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
 * @brief A module of an application depends on a module that is not listed
 * before it.
 */
class MissingDependencyException : public EngineException {
 public:
  MissingDependencyException(const std::string& moduleName,
                             const std::string& dependency);
};

/**
 * @brief A module of an application threw while it started.
 */
class ModuleStartFailedException : public EngineException {
 public:
  ModuleStartFailedException(const std::string& moduleName,
                             const std::string& reason);
};

/**
 * @brief Work was added to a stage the loop does not declare.
 */
class UnknownStageException : public EngineException {
 public:
  explicit UnknownStageException(const std::string& stageName);
};

/**
 * @brief A module provides a service that a module installed before it already
 * provides.
 */
class DuplicateServiceException : public EngineException {
 public:
  DuplicateServiceException(const std::string& moduleName,
                            const std::string& serviceName);
};

/**
 * @brief A module requires a service that no module installed before it
 * provides.
 */
class MissingServiceException : public EngineException {
 public:
  MissingServiceException(const std::string& moduleName,
                          const std::string& serviceName);
};

/**
 * @brief A command was registered under a name another command already has.
 */
class DuplicateCommandException : public EngineException {
 public:
  explicit DuplicateCommandException(const std::string& commandName);
};

/**
 * @brief A setting was declared under a name another setting already has.
 */
class DuplicateSettingException : public EngineException {
 public:
  explicit DuplicateSettingException(const std::string& settingName);
};

/**
 * @brief What a command handler throws to refuse the words it was given; the
 * interpreter turns it into a message naming the command and the line.
 */
class CommandRefusedException : public EngineException {
 public:
  explicit CommandRefusedException(const std::string& reason);
};

/** @brief The system did not tell where the running executable is. */
class UnknownExecutablePathException : public EngineException {
 public:
  UnknownExecutablePathException();
};

/** @brief None of the searched folders is an assets folder. */
class AssetFolderNotFoundException : public EngineException {
 public:
  explicit AssetFolderNotFoundException(
      const std::vector<std::string>& searchedFolders);
};

/**
 * @brief A data file cannot be opened, or a folder of data files cannot be
 * listed.
 */
class UnreadableDataFileException : public EngineException {
 public:
  /** @param file The path of the file or the folder, as the errors show it. */
  explicit UnreadableDataFileException(const std::string& file);
};

/** @brief A data file is not TOML. */
class DataFileSyntaxException : public EngineException {
 public:
  /** @param description What the TOML reader found wrong at @p place. */
  DataFileSyntaxException(const DataFilePlace& place,
                          const std::string& description);
};

/**
 * @brief A key a data file must hold is missing; the place is the table it
 * is missing from.
 */
class MissingDataFileKeyException : public EngineException {
 public:
  explicit MissingDataFileKeyException(const DataFilePlace& place);
};

/** @brief A data file holds a key it may not hold, often a misspelt one. */
class UnknownDataFileKeyException : public EngineException {
 public:
  explicit UnknownDataFileKeyException(const DataFilePlace& place);
};

/** @brief A key of a data file that must hold a table holds a value. */
class DataFileTableExpectedException : public EngineException {
 public:
  explicit DataFileTableExpectedException(const DataFilePlace& place);
};

/** @brief A key of a data file that must hold an array of tables holds
 * something else, or an array with a value that is not a table. */
class DataFileTablesExpectedException : public EngineException {
 public:
  explicit DataFileTablesExpectedException(const DataFilePlace& place);
};

/**
 * @brief A value of a data file is not a TOML integer within the range of
 * its key.
 */
class DataFileValueOutOfRangeException : public EngineException {
 public:
  DataFileValueOutOfRangeException(const DataFilePlace& place,
                                   WholeNumberRange range);
};

/** @brief A key of a data file that must hold a text holds something else. */
class DataFileTextExpectedException : public EngineException {
 public:
  explicit DataFileTextExpectedException(const DataFilePlace& place);
};

/**
 * @brief A key of a data file holds a text that is none of the values it
 * accepts.
 */
class UnknownDataFileChoiceException : public EngineException {
 public:
  /**
   * @param value The text the key holds, in the message.
   * @param choices The values the key accepts, in the message.
   */
  UnknownDataFileChoiceException(const DataFilePlace& place,
                                 const std::string& value,
                                 std::span<const std::string_view> choices);
};

/** @brief A shared library could not be opened. */
class SharedLibraryOpenException : public EngineException {
 public:
  SharedLibraryOpenException(const std::string& file,
                             const std::string& reason);
};

/** @brief A plugin line names a plugin whose file is not in the plugin folder.
 */
class PluginNotFoundException : public EngineException {
 public:
  PluginNotFoundException(const std::string& name, const std::string& file);
};

/** @brief A plugin library does not export the function that creates its
 * module. */
class PluginEntryNotFoundException : public EngineException {
 public:
  PluginEntryNotFoundException(const std::string& name,
                               const std::string& entryName);
};

/** @brief A plugin was built for a plugin API version other than the one the
 * program loads. */
class PluginApiVersionException : public EngineException {
 public:
  PluginApiVersionException(const std::string& name,
                            std::uint32_t pluginVersion,
                            std::uint32_t programVersion);
};

/** @brief A plugin line names something that is not a plugin name. */
class InvalidPluginNameException : public EngineException {
 public:
  explicit InvalidPluginNameException(const std::string& name);
};

/** @brief A module that is added to an application has the name of a module
 * already listed. */
class DuplicateModuleNameException : public EngineException {
 public:
  explicit DuplicateModuleNameException(const std::string& name);
};

}  // namespace rtype::engine
