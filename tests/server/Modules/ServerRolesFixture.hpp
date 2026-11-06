#pragma once

#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include "CommandResult.hpp"
#include "GameLaunchOptions.hpp"
#include "IModule.hpp"
#include "ServerModuleFixture.hpp"
#include "StopRequest.hpp"

/** @brief The base of the tests that keep the roles of the server apart: the
 * modules of a supervisor and of a game process, the settings each installs,
 * and the settings server.cfg.example shows. */
class ServerRolesFixture : public ServerModuleFixture {
 public:
  ServerRolesFixture();

 protected:
  /** @returns The names of the settings the modules of a supervisor declare
   * once installed. */
  [[nodiscard]] std::set<std::string> supervisorSettings();

  /** @returns The names of the settings the modules of a game process
   * declare once installed, the game-statistics plugin loaded. */
  [[nodiscard]] std::set<std::string> gameSettings();

  /** @returns The modules of a game process. */
  [[nodiscard]] std::vector<std::unique_ptr<rtype::engine::IModule>>
  gameModules();

  /** @returns The name of every `set` line of server.cfg.example, the
   * commented ones included. */
  [[nodiscard]] static std::set<std::string> exampleSettings();

 private:
  /** @brief Installs @p modules in an application whose configuration step
   * runs @p pluginLines, then refuses the start once the settings are
   * declared, so no module starts and nothing is opened.
   * @returns The names the refusal of an undeclared setting listed. */
  [[nodiscard]] static std::set<std::string> declaredSettingsOf(
      std::vector<std::unique_ptr<rtype::engine::IModule>> modules,
      const std::vector<std::string>& pluginLines);

  /** @returns The names listed after KNOWN_SETTINGS_MARKER in @p refusal;
   * none when it holds no such list. */
  [[nodiscard]] static std::set<std::string> namesListedIn(
      const rtype::engine::CommandResult& refusal);

  /** @returns @p line without the `#` and the spaces it starts with. */
  [[nodiscard]] static std::string_view uncommented(std::string_view line);

  [[nodiscard]] rtype::server::GameLaunchOptions gameOptions() const;

  std::string journalText_;
  rtype::server::StopRequest stopRequest_;
  std::vector<std::string> gameLines_;
};
