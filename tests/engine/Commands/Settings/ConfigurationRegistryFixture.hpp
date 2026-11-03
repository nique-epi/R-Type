#pragma once

#include <gtest/gtest.h>
#include <string_view>
#include "CommandInterpreter.hpp"
#include "CommandResult.hpp"
#include "ConfigurationRegistry.hpp"
#include "SettingValue.hpp"

/**
 * @brief An interpreter and its registry, with a port, a seed and a nickname
 * declared.
 */
class ConfigurationRegistryFixture : public ::testing::Test {
 protected:
  ConfigurationRegistryFixture();

  /** @returns The result of the line `set <name> <value>`. */
  [[nodiscard]] rtype::engine::CommandResult set(std::string_view name,
                                                 std::string_view value) const;

  /** @returns The interpreter the `set` command is registered in. */
  [[nodiscard]] const rtype::engine::CommandInterpreter& interpreter() const;

  /** @returns The registry the settings are declared in. */
  [[nodiscard]] rtype::engine::ConfigurationRegistry& registry();

  /** @returns The port setting. */
  [[nodiscard]] const rtype::engine::SettingValue& port() const;

  /** @returns The setting that may hold nothing. */
  [[nodiscard]] const rtype::engine::SettingValue& seed() const;

  /** @returns The nickname setting. */
  [[nodiscard]] const rtype::engine::SettingValue& nickname() const;

 private:
  rtype::engine::CommandInterpreter interpreter_;
  rtype::engine::ConfigurationRegistry registry_{interpreter_};
  const rtype::engine::SettingValue* port_;
  const rtype::engine::SettingValue* seed_;
  const rtype::engine::SettingValue* nickname_;
};
