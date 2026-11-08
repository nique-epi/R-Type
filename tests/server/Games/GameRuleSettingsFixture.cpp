#include "GameRuleSettingsFixture.hpp"
#include <string_view>
#include "CommandResult.hpp"
#include "GameRules.hpp"

GameRuleSettingsFixture::GameRuleSettingsFixture() {
  settings_.declare(configuration_);
}

rtype::engine::CommandResult GameRuleSettingsFixture::execute(
    std::string_view line) {
  return commands_.execute(line);
}

rtype::network::GameRules GameRuleSettingsFixture::rules() const {
  return settings_.rules();
}
