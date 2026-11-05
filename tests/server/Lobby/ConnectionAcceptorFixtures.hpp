#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>
#include "EntityTypeEntry.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server::testing {

/**
 * @returns A datagram holding a ConnectionRequest whose nickname is empty,
 * which its reader refuses.
 */
std::vector<std::byte> emptyNicknameRequestDatagram();

/** @returns A nickname of its own for the player at @p player. */
std::string nicknameOf(std::size_t player);

/** @returns How many of @p answers hold a ConnectionAccepted. */
[[nodiscard]] std::size_t acceptancesAmong(
    std::span<const network::OutgoingDatagram> answers);

/** @returns The largest table of entity types the protocol allows: 32 enemy
 * types, each with a name of 24 bytes. */
[[nodiscard]] std::vector<network::EntityTypeEntry> largestEntityTypeTable();

}  // namespace rtype::server::testing
