#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "Endpoint.hpp"
#include "PacketHeader.hpp"
#include "ServerLobbyTestConstants.hpp"

/** @returns Where the player the sorter tests admit sends from. */
[[nodiscard]] rtype::network::Endpoint player();

/** @returns Where a sender the sorter never admits sends from. */
[[nodiscard]] rtype::network::Endpoint stranger();

/** @returns Where a second admitted player sends from. */
[[nodiscard]] rtype::network::Endpoint secondPlayer();

/** @returns A header carrying @p sessionToken, as an admitted player sends. */
[[nodiscard]] rtype::network::PacketHeader playerHeader(
    std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);

/** @returns A ReadyState carrying @p sessionToken. */
[[nodiscard]] std::vector<std::byte> playerReady(
    std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);
