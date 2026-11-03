#pragma once

#include <map>
#include <span>
#include <string>
#include "CommandInterpreter.hpp"
#include "IntegerSettingDefinition.hpp"
#include "SettingValue.hpp"
#include "TextSettingDefinition.hpp"

namespace rtype::engine {

/**
 * @brief The settings a program declares, and the `set` command that gives
 * them a value.
 *
 * A part of the program declares its settings when it installs and reads them
 * when it starts, once the configuration was read. Each value is checked
 * against its bounds before it is kept, so a refused `set` leaves the old
 * value.
 */
class ConfigurationRegistry {
 public:
  /**
   * @brief Registers the `set` command in @p interpreter, which must outlive
   * the registry.
   */
  explicit ConfigurationRegistry(CommandInterpreter& interpreter);

  ConfigurationRegistry(const ConfigurationRegistry&) = delete;
  ConfigurationRegistry& operator=(const ConfigurationRegistry&) = delete;
  ConfigurationRegistry(ConfigurationRegistry&&) = delete;
  ConfigurationRegistry& operator=(ConfigurationRegistry&&) = delete;
  ~ConfigurationRegistry() = default;

  /**
   * @returns The value of the new whole-number setting.
   * @throws DuplicateSettingException if a setting already has that name.
   */
  const SettingValue& declareInteger(IntegerSettingDefinition definition);

  /**
   * @returns The value of the new text setting.
   * @throws DuplicateSettingException if a setting already has that name.
   */
  const SettingValue& declareText(TextSettingDefinition definition);

 private:
  void set(std::span<const std::string> arguments);
  void setInteger(const IntegerSettingDefinition& definition,
                  const std::string& text);
  void setText(const TextSettingDefinition& definition,
               const std::string& text);
  [[nodiscard]] std::string knownNames() const;

  std::map<std::string, SettingValue> values_;
  std::map<std::string, IntegerSettingDefinition> integers_;
  std::map<std::string, TextSettingDefinition> texts_;
};

}  // namespace rtype::engine
