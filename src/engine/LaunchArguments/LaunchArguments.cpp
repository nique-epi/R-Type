#include "LaunchArguments.hpp"
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "EngineException.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LaunchOptionReader.hpp"

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

/**
 * @brief Calls the record of @p option, the first of @p arguments, with the
 * argument after it when the option takes a value.
 * @returns how many arguments the option took, its value included.
 * @throws MissingLaunchOptionValueException when it takes a value and is the
 * last of @p arguments.
 */
std::size_t recordOption(const LaunchOption& option,
                         std::span<const std::string_view> arguments) {
  std::size_t index = 0;
  const std::string_view value =
      option.takesValue ? valueAfter(arguments, index) : std::string_view{};
  option.record(value);
  return index + 1;
}

/**
 * @returns how many arguments the first of @p readers that takes the first of
 * @p arguments took.
 * @throws UnknownLaunchArgumentException when none of them takes it.
 */
std::size_t readWithReaders(std::span<const std::string_view> arguments,
                            std::span<const LaunchOptionReader> readers) {
  for (const LaunchOptionReader& reader : readers) {
    const std::size_t taken = reader(arguments);
    if (taken != 0) {
      return taken;
    }
  }
  throw UnknownLaunchArgumentException(std::string(arguments.front()));
}

}  // namespace

void readLaunchArguments(std::span<const std::string_view> arguments,
                         std::span<const LaunchOption> options,
                         std::span<const LaunchOptionReader> readers) {
  std::size_t index = 0;
  while (index < arguments.size()) {
    const std::span<const std::string_view> rest = arguments.subspan(index);
    const LaunchOption* const option = findLaunchOption(options, rest.front());
    index += option != nullptr ? recordOption(*option, rest)
                               : readWithReaders(rest, readers);
  }
}

std::vector<std::string_view> processArguments(int argumentCount,
                                               const char* const* arguments) {
  std::vector<std::string_view> launchArguments;
  for (int index = 1; index < argumentCount; ++index) {
    launchArguments.emplace_back(arguments[index]);
  }
  return launchArguments;
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

LaunchOption pathOption(std::string_view name,
                        std::optional<std::filesystem::path>& path) {
  return LaunchOption{.name = name,
                      .takesValue = true,
                      .record = [&path](std::string_view value) {
                        path = std::filesystem::path{value};
                      }};
}

}  // namespace rtype::engine
