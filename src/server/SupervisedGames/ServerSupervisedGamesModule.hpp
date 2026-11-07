#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "ExitedGame.hpp"
#include "GameLaunchSettings.hpp"
#include "GameRegistry.hpp"
#include "GameSpawner.hpp"
#include "HostedDifficulties.hpp"
#include "IDatagramSender.hpp"
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "IProcessLauncher.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "SettingValue.hpp"
#include "StopPause.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesWatch.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The module that runs the games of the supervisor, one process each:
 * it owns the process launcher, the registry of the games and the spawner
 * that starts them, and provides the registry and the spawner to the modules
 * listed after it, with what the games hold (see SupervisedGamesWatch): the
 * LatestHostedGames and the LatestServerMeasures the master module sends, and
 * the IServerOccupancy the information module answers. It declares the
 * database setting: the leaderboard every game writes to.
 *
 * In the simulation stage, each tick: the games whose process ended leave the
 * registry (and the network forgets their address); the statuses the games sent
 * are read, each newer one updates its game and is answered with a Heartbeat;
 * a game that waited GAME_START_TIMEOUT without a word is killed; then the
 * list and the measures of the games are published when they changed. A game
 * that
 * crashes only leaves the registry: the supervisor and the other games go on.
 * stop() has every game stopped (see ChildrenStopper) before the network
 * closes.
 *
 * install() opens nothing. start() writes the file of commands of the games
 * (see gameConfigurationFileOf()) with the lines of the configuration that
 * belong to them, once, before a player can ask for a game, and every game is
 * told to read it; processes start when a player asks for one.
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
   * @brief Declares the database setting, reads the services and provides
   * its own.
   * @throws MissingServiceException when the network module did not provide
   * the sender, the admission, the listening port or the datagrams of the
   * tick.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Reads the waves file of the games and keeps the names of its
   * difficulties, then writes GameLaunchSettings::gameLines to the file of
   * commands of the games, replacing it.
   * @throws ServerException, game::GameException or engine::EngineException
   * (or a class derived from one) when the entity sheets or the waves file
   * cannot be read: the supervisor then stops, rather than open games that
   * could not start.
   * @throws GameConfigurationWriteException when the file cannot be written:
   * the supervisor then stops, as it does when its journal or a database
   * cannot be opened, rather than run games without the lines meant for them.
   */
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
  HostedDifficulties difficulties_;
  SupervisedGamesWatch watch_{registry_};
  std::optional<GameSpawner> spawner_;
  logging::Logger logger_;
  const engine::SettingValue* database_{nullptr};
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  const IListeningPort* listeningPort_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
};

}  // namespace rtype::server
