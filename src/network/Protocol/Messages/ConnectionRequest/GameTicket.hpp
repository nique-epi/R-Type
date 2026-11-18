#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include "NetworkConstants.hpp"

namespace rtype::network {

/**
 * @brief The game ticket a ConnectionRequest carries: the GAME_TICKET_BYTES
 * random bytes of a ticket the master gave a signed-in player, or all zeros
 * for a guest. Nothing the server can check: it only reports it to the
 * master with the results of the game, and the master alone knows which
 * account it pays.
 */
struct GameTicket {
  std::array<std::byte, GAME_TICKET_BYTES> bytes{};

  bool operator==(const GameTicket&) const = default;
};

/** @brief The ticket of a guest, a player not signed in to the master. */
constexpr GameTicket GUEST_TICKET{};

/** @returns Whether @p ticket is the one of a guest, all zeros. */
[[nodiscard]] bool isGuestTicket(const GameTicket& ticket);

/** @returns @p ticket as the master writes it: GAME_TICKET_PREFIX, then its
 * bytes in lowercase hexadecimal; nothing for a guest. */
[[nodiscard]] std::optional<std::string> gameTicketText(
    const GameTicket& ticket);

/** @returns The ticket @p text writes as gameTicketText() does; nothing for
 * any other text. */
[[nodiscard]] std::optional<GameTicket> gameTicketOfText(std::string_view text);

}  // namespace rtype::network
