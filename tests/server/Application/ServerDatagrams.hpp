#pragma once

#include <cstddef>
#include <vector>
#include "PacketHeader.hpp"

/** @returns A datagram holding a ConnectionRequest without a cookie. */
[[nodiscard]] std::vector<std::byte> requestDatagram(
    const rtype::network::PacketHeader& header = {});

/** @returns A datagram holding a ReadyState, which only a player sends. */
[[nodiscard]] std::vector<std::byte> readyDatagram(
    const rtype::network::PacketHeader& header = {});
