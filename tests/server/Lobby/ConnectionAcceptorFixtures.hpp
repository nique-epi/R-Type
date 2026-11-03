#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace rtype::server::testing {

/**
 * @returns A datagram holding a ConnectionRequest whose nickname is empty,
 * which its reader refuses.
 */
std::vector<std::byte> emptyNicknameRequestDatagram();

/** @returns A nickname of its own for the requester at @p requester. */
std::string nicknameOf(std::size_t requester);

}  // namespace rtype::server::testing
