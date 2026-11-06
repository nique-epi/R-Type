#pragma once

#include <memory>
#include <string>
#include <vector>
#include "ConnectionAcceptor.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "RunningGame.hpp"
#include "SystemClock.hpp"
#include "TickDatagrams.hpp"

namespace rtype::server {

/**
 * @brief The module that lets players in: it answers the connection requests
 * of each tick by the acceptor of the game, and seats the players it accepts
 * in that game.
 *
 * A player already seated is answered again by the acceptor, so a repeated
 * request that carries a valid cookie finds the same acceptance again, and one
 * without a cookie is challenged; a new player is refused once the game left
 * the waiting room or its seats are all taken.
 *
 * Its work is in the simulation stage, after the games module's: the game
 * moves on first, then the requests of the tick are answered, so a request
 * answered in a tick is refused by the state the game just reached. The
 * acceptor needs the roster of the game, which exists only once the games
 * module started, so start() builds it.
 */
class ServerLobbyModule final : public engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @throws MissingServiceException when no module provided the games, the
   * sender, the admission or the datagrams of the tick.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Builds the acceptor on the roster of the game.
   */
  void start() override;

  /** @brief Drops the acceptor. */
  void stop() override;

 private:
  void answerConnectionRequests();

  engine::SystemClock clock_;
  RunningGame* runningGame_{nullptr};
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
  std::unique_ptr<ConnectionAcceptor> acceptor_;
};

}  // namespace rtype::server
