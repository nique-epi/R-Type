#pragma once

#include "GameRules.hpp"

namespace rtype::engine {

class ConfigurationRegistry;
class SettingValue;

}  // namespace rtype::engine

namespace rtype::server {

/**
 * @brief The settings that hold the rules of the game of a game process:
 * DIFFICULTY_SETTING_NAME, LIVES_SETTING_NAME and FRIENDLY_FIRE_SETTING_NAME.
 * Their defaults are network::defaultGameRules(); the supervisor forces the
 * rules the creator of the game chose, with DIFFICULTY_OPTION, LIVES_OPTION
 * and FRIENDLY_FIRE_OPTION. `set` refuses lives out of LOWEST_LIVES to
 * HIGHEST_LIVES and a friendly fire other than FRIENDLY_FIRE_OFF or
 * FRIENDLY_FIRE_ON. The registry must outlive this object.
 */
class GameRuleSettings {
 public:
  /**
   * @brief Declares the three settings in @p configuration.
   * @throws engine::DuplicateSettingException when one of them is already
   * declared.
   */
  void declare(engine::ConfigurationRegistry& configuration);

  /** @returns The rules the settings hold now; network::defaultGameRules()
   * before declare(). */
  [[nodiscard]] network::GameRules rules() const;

 private:
  const engine::SettingValue* difficulty_{nullptr};
  const engine::SettingValue* lives_{nullptr};
  const engine::SettingValue* friendlyFire_{nullptr};
};

}  // namespace rtype::server
