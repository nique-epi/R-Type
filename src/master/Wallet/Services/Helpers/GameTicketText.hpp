#pragma once

#include <string>
#include <string_view>

namespace rtype::master {

/**
 * @returns A new game ticket: network::GAME_TICKET_PREFIX, then
 * network::GAME_TICKET_BYTES drawn by network::fillWithRandomBytes() in
 * lowercase hexadecimal, as the tokens of the sessions are drawn.
 */
[[nodiscard]] std::string newGameTicket();

/**
 * @returns The fingerprint of @p ticket kept in the database in place of the
 * ticket: its hash by network::hashOf() in GAME_TICKET_FINGERPRINT_CONTEXT, in
 * hexadecimal. A ticket is 256 random bits, so a fast hash is enough, as for
 * the tokens of the sessions.
 */
[[nodiscard]] std::string gameTicketFingerprintOf(std::string_view ticket);

}  // namespace rtype::master
