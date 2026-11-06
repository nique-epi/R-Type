#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "ExitedGame.hpp"
#include "GameLaunchSettings.hpp"
#include "GameRegistry.hpp"
#include "GameSpawner.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "IProcessLauncher.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "StopPause.hpp"
#include "SupervisedGame.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The module that runs the games of the supervisor, one process each:
 * it owns the process launcher, the registry of the games and the spawner
 * that starts them, and provides the registry and the spawner to the modules
 * listed after it.
 *
 * In the simulation stage, each tick: the games whose process ended leave the
 * registry (and the network forgets their address); the statuses the games sent
 * are read, each newer one updates its game and is answered with a Heartbeat;
 * a game that waited GAME_START_TIMEOUT without a word is killed. A game that
 * crashes only leaves the registry: the supervisor and the other games go on.
 * stop() has every game stopped (see ChildrenStopper) before the network
 * closes.
 *
 * install() opens nothing; start() has nothing to open, since processes start
 * when a player asks for one.
 */
class ServerSupervisedGamesModule final : public engine::IModule {
 public:
  /** @param settings What every game process is started with.
   * @param launcher Starts the processes; the module owns it.
   * @param pause How stop() lets time pass while the games end: a sleep. */
  ServerSupervisedGamesModule(GameLaunchSettings settings,
                              std::unique_ptr<IProcessLauncher> launcher,
                              StopPause pause);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @throws MissingServiceException when the network module did not provide
   * the sender, the admission, the listening port or the datagrams of the
   * tick.
   */
  void install(engine::InstallContext& context) override;

  void start() override;

  /** @brief Stops every game, then reports how each ended. */
  void stop() override;

 private:
  void superviseGames(engine::Duration elapsed);
  void readStatuses();
  void answerStatus(const SupervisedGame& game);
  void reportExits(const std::vector<ExitedGame>& exits);

  GameLaunchSettings settings_;
  std::unique_ptr<IProcessLauncher> launcher_;
  StopPause pause_;
  GameRegistry registry_;
  std::optional<GameSpawner> spawner_;
  logging::Logger logger_;
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
};

}  // namespace rtype::server
