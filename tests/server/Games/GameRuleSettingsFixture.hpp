#pragma once

#include <gtest/gtest.h>
#include <string_view>
#include "CommandInterpreter.hpp"
#include "CommandResult.hpp"
#include "ConfigurationRegistry.hpp"
#include "GameRuleSettings.hpp"
#include "GameRules.hpp"

/**
 * @brief The settings of the rules of a game, declared in a registry of their
 * own, which runs the lines a test gives it as a configuration would.
 */
class GameRuleSettingsFixture : public ::testing::Test {
 protected:
  GameRuleSettingsFixture();

  /** @returns What the registry answered to @p line. */
  rtype::engine::CommandResult execute(std::string_view line);
  /** @returns The rules the settings hold now. */
  [[nodiscard]] rtype::network::GameRules rules() const;

 private:
  rtype::engine::CommandInterpreter commands_;
  rtype::engine::ConfigurationRegistry configuration_{commands_};
  rtype::server::GameRuleSettings settings_;
};
