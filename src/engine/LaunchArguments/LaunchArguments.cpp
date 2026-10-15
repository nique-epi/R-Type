#include "LaunchArguments.hpp"
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"

namespace rtype::engine {

namespace {

const LaunchOption* findLaunchOption(std::span<const LaunchOption> options,
                                     std::string_view argument) {
  for (const LaunchOption& option : options) {
    if (option.name == argument) {
      return &option;
    }
  }
  return nullptr;
}

std::string_view valueAfter(std::span<const std::string_view> arguments,
                            std::size_t& index) {
  const std::size_t valueIndex = index + 1;
  if (valueIndex >= arguments.size()) {
    throw MissingLaunchOptionValueException(std::string(arguments[index]));
  }
  index = valueIndex;
  return arguments[valueIndex];
}

}  // namespace

void readLaunchArguments(std::span<const std::string_view> arguments,
                         std::span<const LaunchOption> options) {
  for (std::size_t index = 0; index < arguments.size(); ++index) {
    const LaunchOption* const option =
        findLaunchOption(options, arguments[index]);
    if (option == nullptr) {
      throw UnknownLaunchArgumentException(std::string(arguments[index]));
    }
    const std::string_view value =
        option->takesValue ? valueAfter(arguments, index) : std::string_view{};
    option->record(value);
  }
}

std::string usageHeader(std::string_view programName) {
  return "Usage: " + std::string(programName) + " [options]\n\nOptions:\n";
}

std::string usageEntry(std::string_view option, std::string_view description) {
  return std::string(USAGE_OPTION_INDENT) + std::string(option) + '\n' +
         std::string(USAGE_DESCRIPTION_INDENT) + std::string(description) +
         '\n';
}

std::string launchErrorText(std::string_view programName,
                            std::string_view message) {
  return std::string(programName) + ": " + std::string(message) + "\nRun " +
         std::string(programName) + " " + std::string(HELP_OPTION) +
         " to see the options.\n";
}

}  // namespace rtype::engine
