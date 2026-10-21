#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

/**
 * @brief The players connected to the game, at most MAXIMUM_PLAYERS, each
 * known by the address and port it sends from.
 *
 * A new player takes the lowest number no connected player has, so the first
 * players are numbered FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS in the order
 * they joined. A player keeps its number until it leaves.
 */
class PlayerRoster {
 public:
  PlayerRoster() = default;
  PlayerRoster(const PlayerRoster&) = delete;
  PlayerRoster& operator=(const PlayerRoster&) = delete;
  PlayerRoster(PlayerRoster&&) = delete;
  PlayerRoster& operator=(PlayerRoster&&) = delete;
  ~PlayerRoster() = default;

  /**
   * @returns The player connected from @p endpoint, or nullptr when none is.
   * The player stays at that address as long as the roster lives.
   */
  [[nodiscard]] ConnectedPlayer* find(const network::Endpoint& endpoint);

  /**
   * @returns The player connected under @p playerNumber, or nullptr when none
   * is, including when @p playerNumber is outside FIRST_PLAYER_NUMBER to
   * MAXIMUM_PLAYERS. The player stays at that address as long as the roster
   * lives.
   */
  [[nodiscard]] ConnectedPlayer* findByNumber(std::uint8_t playerNumber);

  /**
   * @returns The connected players, in order of their number. They stay at
   * their addresses as long as the roster lives. The list is kept by the roster
   * and allocates nothing, so a tick may ask for it every time; it shows the
   * players connected when it was asked for, so ask again after add().
   */
  [[nodiscard]] std::span<ConnectedPlayer* const> connected();

  /** @returns Whether MAXIMUM_PLAYERS players are connected. */
  [[nodiscard]] bool isFull() const;

  /**
   * @brief Connects a player sending from @p endpoint, under the lowest free
   * number, its first datagram to send being sequence 0.
   *
   * @param nickname 1 to MAXIMUM_NICKNAME_SIZE bytes of UTF-8.
   * @param sessionToken Never 0.
   * @returns The player connected from @p endpoint: the new one, or the one
   * that already was, unchanged. nullptr when the roster is full and
   * @p endpoint is not in it. The player stays at that address as long as the
   * roster lives.
   */
  ConnectedPlayer* add(const network::Endpoint& endpoint, std::string nickname,
                       std::uint64_t sessionToken);

 private:
  /** @brief Lists in connected_ the players of seats_, in seat order. */
  void listConnected();

  std::array<std::optional<ConnectedPlayer>, network::MAXIMUM_PLAYERS> seats_;
  std::array<ConnectedPlayer*, network::MAXIMUM_PLAYERS> connected_{};
  std::size_t connectedCount_{0};
};

}  // namespace rtype::server
