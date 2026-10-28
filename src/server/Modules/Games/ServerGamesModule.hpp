#pragma once

#include <memory>
#include <string>
#include <vector>
#include "EventBus.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameInstance.hpp"
#include "Games.hpp"
#include "GamesSettings.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"
#include "WavePlan.hpp"

namespace rtype::server {

/**
 * @brief The module that runs the games: the one game of today, its event
 * broadcaster, and the bus and timers they share.
 *
 * Its work in the loop is the order a tick has always had. In the simulation
 * stage, each game reads the datagrams of its players and moves on, and the
 * players whose session closed are forgotten by the network. In the outputs
 * stage, the timers advance and the bus delivers the events of the tick. The
 * lobby module adds its work to the simulation stage after this one.
 *
 * install() builds the bus, the timers and the empty list of games and
 * provides Games. start() reads the waves file, creates the game and
 * subscribes the broadcaster to the bus. stop() tells every player the server
 * shuts down (see FarewellSender) and drops the broadcaster.
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
   * @brief Reads the waves file and creates the first game.
   * @throws ServerException (or a class derived from it) naming the file, the
   * line and the key when the file cannot be read, does not describe the
   * difficulties as the server expects them, or has no DEFAULT_DIFFICULTY.
   */
  void start() override;

  /** @brief Says goodbye to the players, then drops the broadcaster. */
  void stop() override;

 private:
  [[nodiscard]] WavePlan readWaves() const;
  [[nodiscard]] std::unique_ptr<GameInstance> createGame();
  [[nodiscard]] GameEventBroadcaster::Wiring wiringFor(GameInstance& game);
  void playGames();
  void deliverEvents(engine::Duration elapsed);

  GamesSettings settings_;
  logging::Logger logger_;
  engine::EventBus events_;
  engine::TimerScheduler timers_;
  Games games_;
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
  std::unique_ptr<GameEventBroadcaster> broadcaster_;
};

}  // namespace rtype::server
