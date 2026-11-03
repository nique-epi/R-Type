#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "Endpoint.hpp"
#include "PacketHeader.hpp"
#include "ServerLobbyTestConstants.hpp"

/** @returns The address and port of the player the sorter tests admit. */
rtype::network::Endpoint player();

/** @returns An address and port the sorter tests never admit. */
rtype::network::Endpoint stranger();

/** @returns The address and port of a second player. */
rtype::network::Endpoint secondPlayer();

/** @returns A header carrying @p sessionToken, as an admitted player sends. */
rtype::network::PacketHeader playerHeader(
    std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);

/** @returns A ReadyState carrying @p sessionToken. */
std::vector<std::byte> playerReady(
    std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);
