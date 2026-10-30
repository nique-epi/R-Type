#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "Endpoint.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

/**
 * @brief The players connected to the game, at most MAXIMUM_PLAYERS, each
 * known by the address and port it sends from.
 *
 * A new player takes the lowest number no connected player has, so the first
 * players are numbered FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS in the order
 * they joined. A player keeps its number until remove() frees its seat; the
 * next player to join then takes the lowest free number again.
 */
class PlayerRoster {
 public:
  /**
   * @returns The player connected from @p endpoint, or nullptr when none is.
   * The player stays at that address until it is removed.
   */
  [[nodiscard]] ConnectedPlayer* find(const network::Endpoint& endpoint);

  /**
   * @returns The player connected under @p playerNumber, or nullptr when none
   * is, including when @p playerNumber is outside FIRST_PLAYER_NUMBER to
   * MAXIMUM_PLAYERS. The player stays at that address until it is removed.
   */
  [[nodiscard]] ConnectedPlayer* findByNumber(std::uint8_t playerNumber);

  /**
   * @returns The player connected whose ship has the network id @p shipId, or
   * nullptr when none has it, including for NO_ENTITY_ID: a player without a
   * ship owns no ship. The player stays at that address until it is removed.
   */
  [[nodiscard]] ConnectedPlayer* findByShip(std::uint32_t shipId);

  /**
   * @returns The connected players, in order of their number. They stay at
   * their addresses until they are removed. The list is a copy that
   * allocates nothing, so a tick may ask for it every time; a player
   * joining or leaving afterwards leaves it as it was.
   */
  [[nodiscard]] ConnectedPlayers connected();

  /** @returns The address and port of each connected player, in order of
   * their number. */
  [[nodiscard]] std::vector<network::Endpoint> endpoints() const;

  /**
   * @returns Whether a player holds a seat under @p nickname, away or not,
   * comparing the ASCII letters without regard to case and every other byte as
   * it is.
   */
  [[nodiscard]] bool hasNickname(std::string_view nickname) const;

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
   * @p endpoint is not in it. The player stays at that address until it is
   * removed.
   */
  ConnectedPlayer* add(const network::Endpoint& endpoint, std::string nickname,
                       std::uint64_t sessionToken);

  /**
   * @brief Frees the seat of the player connected from @p endpoint, and its
   * number with it. Every pointer to that player is left dangling.
   * @returns The player as it was when its seat was freed, or nothing when no
   * player is connected from @p endpoint.
   */
  std::optional<ConnectedPlayer> remove(const network::Endpoint& endpoint);

 private:
  /** @returns How many seats hold a player, so a list of the players reserves
   * nothing while the roster is empty. */
  [[nodiscard]] std::size_t occupiedSeatCount() const;

  std::array<std::optional<ConnectedPlayer>, network::MAXIMUM_PLAYERS> seats_;
};

}  // namespace rtype::server
