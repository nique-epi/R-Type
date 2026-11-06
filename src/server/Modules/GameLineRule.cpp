#include "GameLineRule.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <set>
#include <span>
#include <string>
#include <utility>
#include "HandOverRule.hpp"
#include "IModule.hpp"
#include "ModuleSwitchesConstants.hpp"
#include "ServerRoleConstants.hpp"
#include "SettingCommandConstants.hpp"

namespace rtype::server {

namespace {

/** @brief Where the name a command acts on sits among the words of a line:
 * `set <name> …`, `module <name> on|off`. */
constexpr std::size_t NAMED_WORD = 1;

/** @returns Whether @p words set a setting of a game. */
bool setsAGameSetting(std::span<const std::string> words) {
  return words.front() == engine::SET_COMMAND_NAME &&
         words.size() > NAMED_WORD &&
         std::ranges::find(GAME_SETTING_NAMES, words[NAMED_WORD]) !=
             GAME_SETTING_NAMES.end();
}

/** @returns Whether @p words run a command only a game runs. */
bool runsAGameCommand(std::span<const std::string> words) {
  return std::ranges::find(GAME_COMMAND_NAMES, words.front()) !=
         GAME_COMMAND_NAMES.end();
}

/** @returns Whether @p words switch a module that is not in
 * @p supervisorNames. */
bool switchesAGameModule(std::span<const std::string> words,
                         const std::set<std::string>& supervisorNames) {
  return words.front() == engine::MODULE_COMMAND_NAME &&
         words.size() > NAMED_WORD &&
         !supervisorNames.contains(words[NAMED_WORD]);
}

}  // namespace

engine::HandOverRule gameLineRule(
    std::span<const std::unique_ptr<engine::IModule>> supervisorModules) {
  std::set<std::string> supervisorNames;
  for (const std::unique_ptr<engine::IModule>& module : supervisorModules) {
    supervisorNames.insert(module->name());
  }
  return [supervisorNames =
              std::move(supervisorNames)](std::span<const std::string> words) {
    return !words.empty() &&
           (setsAGameSetting(words) || runsAGameCommand(words) ||
            switchesAGameModule(words, supervisorNames));
  };
}

}  // namespace rtype::server
