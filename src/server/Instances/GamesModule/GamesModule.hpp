#pragma once

#include <memory>
#include <string>
#include <vector>
#include "EventBus.hpp"
#include "GameEventBroadcaster.hpp"
#include "GameList.hpp"
#include "GameListSettings.hpp"
#include "IModule.hpp"
#include "IServerLink.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "ServerApplication.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"

namespace rtype::server {

/**
 * @brief The games of the server, one today, with their event bus, their
 * timers and the broadcaster of their events.
 *
 * During its install() it requires the IServerLink and the datagrams of the
 * tick, provides the GameList, and adds two works to the loop: in
 * SIMULATION_STAGE, each game reads the datagrams of its players and moves on,
 * then the players whose session closed are forgotten by the network; in
 * OUTPUT_STAGE, the timers advance and the bus delivers the events of the
 * tick. The lobby, listed after it, adds its own work to SIMULATION_STAGE, so
 * the connection requests are answered after the games ticked and before the
 * events are delivered, as the tick always ran.
 *
 * Its start() reads the waves of DEFAULT_DIFFICULTY from the waves file, then
 * builds the bus, the timers, the game and the broadcaster, which subscribe to
 * the bus; a start() that fails destroys what it built. Its stop() first sends
 * every connected player a Disconnection, REPEATED_MESSAGE_COPIES times
 * REPEATED_MESSAGE_INTERVAL apart, as the protocol asks of a server that shuts
 * down, then destroys the broadcaster, the games, the timers and the bus, the
 * subscriptions with it.
 */
class GamesModule : public engine::IModule {
 public:
  explicit GamesModule(GameListSettings settings);

  /** @returns GAMES_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns NETWORK_MODULE_NAME, whose services it requires. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /**
   * @throws MissingServiceException if no module listed before provides the
   * IServerLink or the datagrams of the tick.
   */
  void install(engine::InstallContext& context) override;
  /**
   * @throws ServerException when the waves file cannot be read, does not
   * describe the difficulties as the server expects them, or has no
   * DEFAULT_DIFFICULTY.
   */
  void start() override;
  /**
   * @brief Tells the players the server stops, then destroys the games. Call
   * it once the loop stopped, before the network module stops: it sends
   * through the link. A Disconnection that cannot be sent is logged, never
   * thrown.
   */
  void stop() override;

 private:
  void open();
  void close();
  void playTick();
  void deliverEvents(engine::Duration elapsed);
  void sayGoodbye();

  GameListSettings settings_;
  IServerLink* link_{nullptr};
  const ServerApplication::ReceivedDatagrams* received_{nullptr};
  GameList games_;
  std::unique_ptr<engine::EventBus> events_;
  std::unique_ptr<engine::TimerScheduler> timers_;
  std::unique_ptr<GameEventBroadcaster> broadcaster_;
  logging::Logger logger_;
};

}  // namespace rtype::server
