#pragma once

#include <chrono>
#include <optional>
#include <string_view>
#include "CreatedServer.hpp"
#include "CreationAttempt.hpp"
#include "HeartbeatSampleRepository.hpp"
#include "KnownServer.hpp"
#include "MasterDatabase.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerRecord.hpp"
#include "ServerRepository.hpp"

namespace rtype::master {

/** @brief The rules of the creation, registration and heartbeats of servers. */
class ServersService {
 public:
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
   * @p server, and as a sample of that second.
   * @returns false when @p server has not registered: nothing is recorded.
   * @throws network::InvalidMasterMessageException naming the member of the
   * heartbeat beyond what the server registered: more games than its
   * maximumGames, a game numbered above it, or more players in a game than its
   * maximumPlayersPerGame. Nothing is recorded then.
   */
  bool replaceOwnHeartbeat(const KnownServer& server,
                           network::ServerHeartbeat heartbeat,
                           std::chrono::sys_seconds now);

 private:
  ServerRepository servers_;
  HeartbeatSampleRepository samples_;
};

}  // namespace rtype::master
