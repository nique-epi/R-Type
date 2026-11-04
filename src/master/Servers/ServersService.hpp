#pragma once

#include <chrono>
#include <cstdint>
#include "MasterDatabase.hpp"
#include "RegisteredServer.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerRecord.hpp"
#include "ServerRepository.hpp"

namespace rtype::master {

/** @brief The rules of the registration and the heartbeats of game servers. */
class ServersService {
 public:
  explicit ServersService(MasterDatabase& database);

  /** @brief Registers the server of @p record, or updates it (see save()). */
  RegisteredServer registerServer(const ServerRecord& record);

  /**
   * @brief Records @p heartbeat, received at @p now, of the server the key
   * @p keyId registered.
   * @returns false when that key registered no server: nothing is recorded.
   * @throws network::InvalidMasterMessageException naming the member of the
   * heartbeat beyond what the server registered: more games than its
   * maximumGames, a game numbered above it, or more players in a game than its
   * maximumPlayersPerGame. Nothing is recorded then.
   */
  bool recordHeartbeat(std::int64_t keyId, network::ServerHeartbeat heartbeat,
                       std::chrono::sys_seconds now);

 private:
  ServerRepository repository_;
};

}  // namespace rtype::master
