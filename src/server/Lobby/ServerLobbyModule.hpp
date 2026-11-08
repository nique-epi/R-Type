#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "ConnectionAcceptor.hpp"
#include "ContentAnnouncement.hpp"
#include "IDatagramSender.hpp"
#include "IModule.hpp"
#include "IOptionalDependencies.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "LatestBans.hpp"
#include "PlayerExpeller.hpp"
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
 * module started, so start() builds it. Every acceptance announces the
 * content pack of the content folders, read once while it installs, served by
 * the supervisor on the TCP port it is built with.
 *
 * It keeps the banned pilots out with the LatestBans of the storage module,
 * which it works without: a banned newcomer is refused (Banned), and each
 * time the storage module publishes newer bans, every seated pilot they bar
 * is expelled for the reason Banned (see PlayerExpeller).
 */
class ServerLobbyModule final : public engine::IModule,
                                public engine::IOptionalDependencies {
 public:
  /** @param contentPort The TCP port the supervisor serves the content pack
   * on: the number of its UDP port.
   * @param contentFolders The folders the pack is made of, the assets first
   * then each resource started (see engine::contentFoldersOf()). */
  ServerLobbyModule(std::uint16_t contentPort,
                    std::vector<std::filesystem::path> contentFolders);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  [[nodiscard]] std::vector<std::string> optionalDependencies() const override;

  /**
   * @brief Reads the content of the content folders into the announcement.
   * @throws MissingServiceException when no module provided the games, the
   * sender, the admission or the datagrams of the tick, and what
   * contentAnnouncementOf() throws.
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
  /** @brief Expels the seated pilots the bans bar, once per revision of the
   * bans the storage module published. */
  void expelBannedPilots();

  engine::SystemClock clock_;
  std::uint16_t contentPort_;
  std::vector<std::filesystem::path> contentFolders_;
  network::ContentAnnouncement content_;
  RunningGame* runningGame_{nullptr};
  IDatagramSender* sender_{nullptr};
  IPlayerAdmission* admission_{nullptr};
  TickDatagrams* tickDatagrams_{nullptr};
  std::unique_ptr<ConnectionAcceptor> acceptor_;
  const LatestBans* bans_{nullptr};
  std::uint64_t seenBans_{0};
  PlayerExpeller expeller_;
};

}  // namespace rtype::server
