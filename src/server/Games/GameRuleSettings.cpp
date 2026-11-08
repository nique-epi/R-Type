#include "GameRuleSettings.hpp"
#include <cstdint>
#include <string>
#include "ConfigurationRegistry.hpp"
#include "GameRuleConstants.hpp"
#include "GameRules.hpp"
#include "IntegerSettingDefinition.hpp"
#include "ServerConstants.hpp"
#include "SettingValue.hpp"
#include "TextSettingDefinition.hpp"

namespace rtype::server {

void GameRuleSettings::declare(engine::ConfigurationRegistry& configuration) {
  const network::GameRules defaults = network::defaultGameRules();
  difficulty_ = &configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{DIFFICULTY_SETTING_NAME},
      .defaultValue = defaults.difficulty,
      .help = "Difficulty of the game: a table of the waves file",
      .validate = {}});
  lives_ = &configuration.declareInteger(engine::IntegerSettingDefinition{
      .name = std::string{LIVES_SETTING_NAME},
      .defaultValue = defaults.lives,
      .lowest = identifiers::LOWEST_LIVES,
      .highest = identifiers::HIGHEST_LIVES,
      .help = "Lives each player starts the game with",
      .allowsNone = false});
  friendlyFire_ =
      &configuration.declareInteger(engine::IntegerSettingDefinition{
          .name = std::string{FRIENDLY_FIRE_SETTING_NAME},
          .defaultValue = FRIENDLY_FIRE_OFF,
          .lowest = FRIENDLY_FIRE_OFF,
          .highest = FRIENDLY_FIRE_ON,
          .help = "1 when a missile of a player costs another player a life",
          .allowsNone = false});
}

network::GameRules GameRuleSettings::rules() const {
  if (difficulty_ == nullptr) {
    return network::defaultGameRules();
  }
  const network::GameRules defaults = network::defaultGameRules();
  return network::GameRules{
      .difficulty = difficulty_->text(),
      .lives =
          static_cast<std::uint8_t>(lives_->integer().value_or(defaults.lives)),
      .friendlyFire = friendlyFire_->integer().value_or(FRIENDLY_FIRE_OFF) ==
                      FRIENDLY_FIRE_ON};
}

}  // namespace rtype::server
