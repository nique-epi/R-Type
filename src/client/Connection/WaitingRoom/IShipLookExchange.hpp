#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "PlayerLooks.hpp"
#include "ShipLook.hpp"

namespace rtype::client {

/**
 * @brief The looks of the pilots of the session, as the screens read and drive
 * them: the look the player chose, sent to the server, the look of every pilot
 * the server shows, and the items the player won.
 */
class IShipLookExchange {
 public:
  virtual ~IShipLookExchange() = default;

  /** @brief Asks the server to show @p look on the ship of the player: a
   * value, never a change, sent again until a PlayerLooks shows it. Ignored
   * without a session, once the game started, or when it is the look already
   * asked for. */
  virtual void chooseLook(identifiers::ShipLook look) = 0;

  /** @returns The look of each pilot as the newest PlayerLooks lists it; empty
   * before the first one and without a session. */
  [[nodiscard]] virtual std::span<const network::PlayerLook> playerLooks()
      const = 0;

  /** @returns The numbers of the items the server said the player won since
   * the last call, in the order they came, and forgets them. */
  [[nodiscard]] virtual std::vector<std::uint8_t> takeWonItems() = 0;
};

}  // namespace rtype::client
