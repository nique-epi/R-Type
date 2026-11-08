#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>
#include "ContentAnnouncement.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server::testing {

/**
 * @returns A datagram holding a ConnectionRequest whose nickname is empty,
 * which its reader refuses.
 */
std::vector<std::byte> emptyNicknameRequestDatagram();

/** @returns A nickname of its own for the player at @p player. */
std::string nicknameOf(std::size_t player);

/** @returns The content the acceptor of the tests announces: a fingerprint
 * of GAME_FINGERPRINT_BYTE, of GAME_PACK_SIZE, on GAME_PACK_PORT. */
[[nodiscard]] network::ContentAnnouncement gameContent();

/** @returns How many of @p answers hold a ConnectionAccepted. */
[[nodiscard]] std::size_t acceptancesAmong(
    std::span<const network::OutgoingDatagram> answers);

}  // namespace rtype::server::testing
