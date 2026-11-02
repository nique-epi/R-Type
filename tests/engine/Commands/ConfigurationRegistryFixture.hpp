#pragma once

#include <gtest/gtest.h>
#include <string>
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
  rtype::engine::CommandResult set(std::string_view name,
                                   std::string_view value) const;

  rtype::engine::CommandInterpreter interpreter_;
  rtype::engine::ConfigurationRegistry registry_{interpreter_};
  const rtype::engine::SettingValue* port_;
  const rtype::engine::SettingValue* seed_;
  const rtype::engine::SettingValue* nickname_;
};
