#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string_view>
#include "CookieIssuer.hpp"
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "GameListing.hpp"
#include "HeartbeatSampleRepository.hpp"
#include "KnownServer.hpp"
#include "MasterDatabase.hpp"
#include "OutgoingDatagram.hpp"
#include "PageRequest.hpp"
#include "ProbeTarget.hpp"
#include "SampleListing.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerListing.hpp"
#include "ServerRecord.hpp"
#include "ServerRepository.hpp"
#include "StatusChangeRepository.hpp"
#include "StatusListing.hpp"

namespace rtype::master {

/** @brief The rules of the creation, registration and heartbeats of servers. */
class ServersService {
 public:
  /** @brief Draws the key of the tokens of its probes from
   * std::random_device: the tokens of a master that restarted are refused. */
  explicit ServersService(MasterDatabase& database);

  /**
   * @brief Creates the server @p attempt asks for, with a new key whose
   * fingerprint alone is kept.
   * @returns The server and its key, which the master never shows again; or
   * nothing when the address of @p attempt created
   * MAXIMUM_CREATIONS_PER_ADDRESS servers in the last CREATION_WINDOW.
   */
  std::optional<network::CreatedServer> createServer(
      const CreationAttempt& attempt);

  /**
   * @returns The server whose key is @p key, or nothing for a key that is
   * not well formed (network::isWellFormedServerKey()) or that no server has.
   */
  [[nodiscard]] std::optional<KnownServer> serverOfKey(std::string_view key);

  /** @brief Replaces the representation of a server (see saveRegistration()).
   */
  void replaceOwnServer(const ServerRecord& record);

  /**
   * @brief Records @p heartbeat, received at @p now, as the last state of
   * @p server, and as a sample of that second, in one transaction: a failure
   * keeps neither, and the server's last state stays the heartbeat before.
   * When it carries a valid token of a probe (answersProbe()), @p now becomes
   * the last answer of the server.
   * @returns false when @p server has not registered, or registered before the
   * probes and has no secret: nothing is recorded, and the server must
   * register again.
   * @throws network::InvalidMasterMessageException naming the member of the
   * heartbeat beyond what the server registered: more games than its
   * maximumGames, a game numbered above it, or more players in a game than its
   * maximumPlayersPerGame. Nothing is recorded then.
   */
  bool replaceOwnHeartbeat(const KnownServer& server,
                           network::ServerHeartbeat heartbeat,
                           std::chrono::sys_seconds now);

  /**
   * @returns The probe to send @p server at @p now: its secret, and the token
   * of its address and port in the window of @p now; nothing when it has no
   * probe target.
   */
  [[nodiscard]] std::optional<network::OutgoingDatagram> probeFor(
      const KnownServer& server, std::chrono::sys_seconds now) const;

  /**
   * @returns The games the server @p serverId hosts, as the last heartbeat
   * lists them, or nothing when no server has that id. A server that sent no
   * heartbeat since it registered lists none.
   * @throws network::InvalidMasterMessageException when the heartbeat kept no
   * longer reads (see ServerRepository::lastHeartbeatOf()).
   */
  [[nodiscard]] std::optional<network::GameListing> gameListingOf(
      std::int64_t serverId);

  /**
   * @returns The registered servers of @p page, by id, each with the status
   * the master computed last and the counts of its last heartbeat.
   */
  [[nodiscard]] network::ServerListing serverListingOf(const PageRequest& page);

  /** @returns The samples of the server @p serverId in @p page, newest first,
   * or nothing when no server has that id. */
  [[nodiscard]] std::optional<network::SampleListing> sampleListingOf(
      std::int64_t serverId, const PageRequest& page);

  /** @returns The statuses the server @p serverId held in @p page,
   * newest first, or nothing when no server has that id. */
  [[nodiscard]] std::optional<network::StatusListing> statusListingOf(
      std::int64_t serverId, const PageRequest& page);

 private:
  /** @returns Whether @p token is a token of a probe to @p target still
   * valid at @p now (PROBE_TOKEN_WINDOWS); false for no token. */
  [[nodiscard]] bool answersProbe(std::optional<std::uint64_t> token,
                                  const ProbeTarget& target,
                                  std::chrono::sys_seconds now) const;

  MasterDatabase& database_;
  network::CookieIssuer probeTokens_;
  ServerRepository servers_;
  HeartbeatSampleRepository samples_;
  StatusChangeRepository changes_;
};

}  // namespace rtype::master
