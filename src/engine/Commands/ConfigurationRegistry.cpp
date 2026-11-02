#include "ConfigurationRegistry.hpp"
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <system_error>
#include <utility>
#include <vector>
#include "CommandDefinition.hpp"
#include "CommandInterpreter.hpp"
#include "CommandsConstants.hpp"
#include "EngineException.hpp"
#include "IntegerSettingDefinition.hpp"
#include "NameList.hpp"
#include "SettingValue.hpp"
#include "TextSettingDefinition.hpp"

namespace rtype::engine {

namespace {

constexpr std::size_t SET_ARGUMENT_COUNT = 2;

/**
 * @returns The whole number @p text is made of, digits only.
 * @throws CommandRefusedException when it is anything else, or too large.
 */
std::int64_t wholeNumberFrom(const std::string& text) {
  std::int64_t number = 0;
  const char* const end = text.data() + text.size();
  const auto [stop, error] = std::from_chars(text.data(), end, number);
  if (text.empty() || error != std::errc{} || stop != end) {
    throw CommandRefusedException("value '" + text + "' is not a whole number");
  }
  return number;
}

/**
 * @throws CommandRefusedException when @p number is outside the bounds of
 * @p definition.
 */
void requireWithinBounds(const IntegerSettingDefinition& definition,
                         std::int64_t number) {
  if (number < definition.lowest || number > definition.highest) {
    throw CommandRefusedException("value " + std::to_string(number) +
                                  " is outside " +
                                  std::to_string(definition.lowest) + ".." +
                                  std::to_string(definition.highest));
  }
}

}  // namespace

ConfigurationRegistry::ConfigurationRegistry(CommandInterpreter& interpreter) {
  interpreter.add(
      CommandDefinition{.name = std::string(SET_COMMAND_NAME),
                        .arguments = "<setting> <value>",
                        .help = "Gives a declared setting a value"},
      [this](std::span<const std::string> arguments) { set(arguments); });
}

const SettingValue& ConfigurationRegistry::declareInteger(
    IntegerSettingDefinition definition) {
  if (values_.contains(definition.name)) {
    throw DuplicateSettingException(definition.name);
  }
  const std::string name = definition.name;
  const auto inserted =
      values_.emplace(name, SettingValue{definition.defaultValue, {}});
  integers_.emplace(name, std::move(definition));
  return inserted.first->second;
}

const SettingValue& ConfigurationRegistry::declareText(
    TextSettingDefinition definition) {
  if (values_.contains(definition.name)) {
    throw DuplicateSettingException(definition.name);
  }
  const std::string name = definition.name;
  const auto inserted = values_.emplace(
      name, SettingValue{std::nullopt, definition.defaultValue});
  texts_.emplace(name, std::move(definition));
  return inserted.first->second;
}

void ConfigurationRegistry::set(std::span<const std::string> arguments) {
  if (arguments.size() != SET_ARGUMENT_COUNT) {
    throw CommandRefusedException("expected: set <setting> <value>");
  }
  const std::string& name = arguments[0];
  if (!values_.contains(name)) {
    throw CommandRefusedException("unknown setting '" + name +
                                  "'; known settings: " + knownNames());
  }
  const auto integer = integers_.find(name);
  if (integer != integers_.end()) {
    setInteger(integer->second, arguments[1]);
  } else {
    setText(texts_.at(name), arguments[1]);
  }
}

void ConfigurationRegistry::setInteger(
    const IntegerSettingDefinition& definition, const std::string& text) {
  std::optional<std::int64_t> number;
  if (definition.allowsNone && text == NO_VALUE_WORD) {
    number = std::nullopt;
  } else {
    number = wholeNumberFrom(text);
    requireWithinBounds(definition, *number);
  }
  values_.at(definition.name).integer_ = number;
}

void ConfigurationRegistry::setText(const TextSettingDefinition& definition,
                                    const std::string& text) {
  if (definition.validate) {
    definition.validate(text);
  }
  values_.at(definition.name).text_ = text;
}

std::string ConfigurationRegistry::knownNames() const {
  std::vector<std::string> names;
  names.reserve(values_.size());
  for (const auto& entry : values_) {
    names.push_back(entry.first);
  }
  return joinedNames(names);
}

}  // namespace rtype::engine
