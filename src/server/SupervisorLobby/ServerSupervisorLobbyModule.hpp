#pragma once

#include <string>
#include <vector>
#include "ConnectionRequest.hpp"
#include "CookieIssuer.hpp"
#include "GameRegistry.hpp"
#include "GameSpawner.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "SeatRequest.hpp"
#include "SystemClock.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The module that answers the players who ask the supervisor for a
 * place. A request without a valid cookie gets a challenge and the supervisor
 * keeps nothing: only a player who receives at its address can send the cookie
 * back, so only a valid request makes the supervisor do anything that costs
 * (start a process). A valid request is sent where the registry says for
 * the game it asks for (see GameRegistry::placeFor()): to the game that has a
 * place (ConnectionRedirect), nowhere yet while that game starts (the player
 * asks again every CONNECTION_REQUEST_INTERVAL), to a new game when it needs
 * one and fewer than MAXIMUM_RUNNING_GAMES run, and refused with the reason
 * the registry gives otherwise (ServerFull, GameNotFound, GameInProgress,
 * GameFull). A game opened for a NewGame remembers its sender, so the request
 * it repeats opens no second game. The registry holds a seat for each player
 * it sends to a game, or makes wait for one, the player whose request opened a
 * game included, so a burst of requests fills a game and opens the next
 * instead of sending a fifth player to a game of four. A request of another
 * protocol version is refused as the games refuse it.
 *
 * Its work is in the simulation stage, after the supervised games module's.
 */
class ServerSupervisorLobbyModule final : public engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @throws MissingServiceException when no module provided the registry, the
   * spawner, the sender or the datagrams of the tick.
   */
  void install(engine::InstallContext& context) override;

  void start() override;
  void stop() override;

 private:
  void answerConnectionRequests();
  void answer(const network::IncomingDatagram& datagram, engine::Duration now);
  void refuseAnotherVersion(const network::IncomingDatagram& datagram);
  void answerValidRequest(const SeatRequest& request,
                          const network::GameChoice& game);

  engine::SystemClock clock_;
  network::CookieIssuer cookies_;
  GameRegistry* registry_{nullptr};
  GameSpawner* spawner_{nullptr};
  IDatagramSender* sender_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
};

}  // namespace rtype::server
