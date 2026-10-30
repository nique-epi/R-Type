#pragma once

#include <memory>
#include <string>
#include <vector>
#include "EventBus.hpp"
#include "GameInstance.hpp"
#include "GameSettings.hpp"
#include "Games.hpp"
#include "GamesSettings.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"
#include "WavePlan.hpp"

namespace rtype::server {

/**
 * @brief The module that runs the games: GamesSettings::gameCount games, each
 * with its own bus, and the timers they share.
 *
 * Every game runs on the simulation thread, one after the other in each tick:
 * no state is shared between games, so nothing needs a lock, but an exception
 * in one game stops the server and a slow game slows the others. In the
 * simulation stage, each game reads the datagrams of its own players (see
 * keepDatagramsOf()) and moves on, and the players whose session closed are
 * forgotten by the network. In the outputs stage, the timers advance and the
 * bus of each game delivers the events of the tick. The lobby module adds its
 * work to the simulation stage after this one.
 *
 * install() builds the timers and the empty list of games and provides Games.
 * start() reads the waves file once, then creates the games, each subscribing
 * its rules and its broadcaster to its own bus. A finished game waits for
 * players again, so the games are never replaced. stop() tells every player
 * the server shuts down (see FarewellSender).
 */
class ServerGamesModule final : public engine::IModule {
 public:
  explicit ServerGamesModule(GamesSettings settings);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @throws MissingServiceException when the network module did not provide
   * the sender, the admission or the datagrams of the tick.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Reads the waves file and creates the games. With a seed, game n
   * (from 0) starts from the seed plus n, so each game can be played again;
   * without one, each game draws its own.
   * @throws ServerException (or a class derived from it) naming the file, the
   * line and the key when the file cannot be read, does not describe the
   * difficulties as the server expects them, or has no DEFAULT_DIFFICULTY.
   */
  void start() override;

  /** @brief Says goodbye to the players, then drops the broadcaster. */
  void stop() override;

 private:
  [[nodiscard]] WavePlan readWaves() const;
  [[nodiscard]] std::unique_ptr<GameInstance> createGame(
      engine::EventBus& events, GameSettings settings);
  void playGames();
  void deliverEvents(engine::Duration elapsed);

  GamesSettings settings_;
  logging::Logger logger_;
  engine::TimerScheduler timers_;
  Games games_;
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
  std::vector<network::IncomingDatagram> datagramsOfGame_;
};

}  // namespace rtype::server
