#pragma once

#include <memory>
#include <string>
#include <vector>
#include "ConnectionAcceptor.hpp"
#include "Games.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "SystemClock.hpp"
#include "TickDatagrams.hpp"

namespace rtype::server {

/**
 * @brief The module that lets players in: it answers the connection requests
 * of each tick, each by the acceptor of the game Games::placeOf() names for
 * its sender, and seats the players it accepts in that game.
 *
 * A player already seated is answered by the acceptor of its own game, so a
 * repeated request that carries a valid cookie finds the same acceptance
 * again, and one without a cookie is challenged; a new player goes to the
 * first game waiting for players with a free seat. Each acceptor draws its own
 * cookies, so a player challenged by one game and sent to another, because
 * the first one filled up meanwhile, is challenged once more.
 *
 * Its work is in the simulation stage, after the games module's: the games
 * move on first, then the requests of the tick are answered, so a request
 * answered in a tick is placed and refused by the state the games just
 * reached. The acceptors need the rosters of the games, which exist only once
 * the games module started, so start() builds them.
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
   * @brief Builds one acceptor on the roster of each game.
   */
  void start() override;

  /** @brief Drops the acceptors. */
  void stop() override;

 private:
  void answerConnectionRequests();

  engine::SystemClock clock_;
  Games* games_{nullptr};
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
  /** @brief One acceptor per game, in the order of Games::all(). */
  std::vector<std::unique_ptr<ConnectionAcceptor>> acceptors_;
};

}  // namespace rtype::server
