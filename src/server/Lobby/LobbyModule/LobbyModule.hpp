#pragma once

#include <memory>
#include <string>
#include <vector>
#include "ConnectionAcceptor.hpp"
#include "GameList.hpp"
#include "IModule.hpp"
#include "IServerLink.hpp"
#include "InstallContext.hpp"
#include "ServerApplication.hpp"
#include "SystemClock.hpp"

namespace rtype::server {

/**
 * @brief The lobby of the server: the ConnectionAcceptor that answers the
 * connection requests and seats each new player in the game GameList names
 * joinable, the first one.
 *
 * During its install() it requires the IServerLink, the datagrams of the tick
 * and the GameList, and adds its work to SIMULATION_STAGE, after the work of
 * the games: it tells the acceptor whether the joinable game is in progress,
 * then hands it the connection requests of the tick, with the time of the
 * SystemClock for their cookies. A player it lets in is admitted by the
 * network, and the joinable game gives it a ship.
 *
 * Its start() builds the acceptor on the roster of the joinable game, which
 * exists once the games module started; its stop() destroys it.
 */
class LobbyModule : public engine::IModule {
 public:
  /** @returns LOBBY_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns NETWORK_MODULE_NAME and GAMES_MODULE_NAME, whose services it
   * requires. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /**
   * @throws MissingServiceException if no module listed before provides the
   * IServerLink, the datagrams of the tick or the GameList.
   */
  void install(engine::InstallContext& context) override;
  /** @throws NoGameException when the games module started no game. */
  void start() override;
  void stop() override;

 private:
  void answerRequests();

  IServerLink* link_{nullptr};
  const ServerApplication::ReceivedDatagrams* received_{nullptr};
  GameList* games_{nullptr};
  engine::SystemClock clock_;
  std::unique_ptr<ConnectionAcceptor> acceptor_;
};

}  // namespace rtype::server
