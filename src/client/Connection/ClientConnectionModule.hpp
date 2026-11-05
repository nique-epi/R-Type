#pragma once

#include <string>
#include <vector>
#include "EntityAppearances.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "NetworkConditions.hpp"
#include "PlayerConnection.hpp"

namespace rtype::client {

/**
 * @brief The module of the connection to a server: the PlayerConnection of the
 * client, whose parts it provides to the modules installed after it: the
 * ServerSession (the attempt and the session), the WaitingRoom, the
 * EntityMessageQueue, the ServerLeaderboard, IPlayerRoundTrips, the round
 * trip of each player, and IEntityTypeTable, the entity types of the server.
 * It also provides the network::TrafficCounters of its socket.
 *
 * install() requires the EntityAppearances of the assets module: an
 * acceptance whose table names a type none of them draws ends the attempt as
 * MissingEntitySheet. It provides those parts, over a socket still closed,
 * and adds to NETWORK_STAGE the work that reads what the server sent and sends
 * what is due. start() opens the socket on every IPv4 interface, on a port the
 * system picks, and starts the network thread. stop() leaves the session with
 * the server, if any (see PlayerConnection::leave()): it returns once every
 * copy of the Disconnection went out, so the server frees the place of the
 * player as soon as one copy reaches it; then it closes the socket. Over a
 * simulated network, the copies still in flight then never arrive (see
 * NetworkThread::close()), and the server frees the place once the player was
 * silent for SESSION_SILENCE_LIMIT, counted in the ticks it runs, so longer
 * when the server stalls. A failure while leaving is logged, never thrown.
 */
class ClientConnectionModule final : public engine::IModule {
 public:
  /** @param conditions The network the datagrams of the client cross, read at
   * launch. */
  explicit ClientConnectionModule(const network::NetworkConditions& conditions);

  /** @returns CONNECTION_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns LOGGER_MODULE_NAME, since the connection logs from its first
   * frame on, and ASSETS_MODULE_NAME, whose EntityAppearances it reads. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException when the assets module did not provide
   * the EntityAppearances. */
  void install(engine::InstallContext& context) override;
  /** @throws SocketOpenException when the system refuses to open the
   * socket. */
  void start() override;
  void stop() override;

 private:
  const EntityAppearances* appearances_{nullptr};
  PlayerConnection connection_;
  logging::Logger logger_;
};

}  // namespace rtype::client
