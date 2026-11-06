#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace rtype::network {

/**
 * @brief What a server tells the master when it registers: where the players
 * reach it, what it runs and how many it takes.
 */
struct ServerRegistration {
  /** @brief The name players read, MAXIMUM_SERVER_NAME_SIZE bytes at most. */
  std::string name;
  /**
   * @brief Its IPv4 address; empty when the server does not know it, and the
   * master then takes the address the request came from.
   */
  std::optional<std::string> address;
  std::uint16_t udpPort{};
  /** @brief The game version it runs, MAXIMUM_GAME_VERSION_SIZE at most. */
  std::string gameVersion;
  /** @brief The version of the UDP protocol it speaks, PROTOCOL_VERSION. */
  std::uint8_t protocolVersion{};
  /** @brief The games it hosts at most, MAXIMUM_HOSTED_GAMES at most. */
  std::uint16_t maximumGames{};
  /** @brief The players one of its games takes, MAXIMUM_PLAYERS at most. */
  std::uint8_t maximumPlayersPerGame{};
  /**
   * @brief The secret the probes of the master must carry, from
   * LOWEST_PROBE_SECRET to HIGHEST_PROBE_SECRET. Written by registrationText()
   * only: the list of servers never shows it, and a registration read back
   * from the database leaves it 0.
   */
  std::uint64_t probeSecret{};

  bool operator==(const ServerRegistration&) const = default;
};

}  // namespace rtype::network
