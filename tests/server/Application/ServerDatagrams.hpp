#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"

/** @returns A datagram holding a ConnectionRequest without a cookie. */
[[nodiscard]] std::vector<std::byte> requestDatagram(
    const rtype::network::PacketHeader& header = {});

/** @returns A datagram holding a ConnectionRequest that carries @p cookie. */
[[nodiscard]] std::vector<std::byte> requestDatagram(
    std::uint64_t cookie, const rtype::network::PacketHeader& header = {});

/** @returns A datagram holding a ReadyState, which only a player sends. */
[[nodiscard]] std::vector<std::byte> readyDatagram(
    const rtype::network::PacketHeader& header = {});

/**
 * @returns How a log line names @p sender: "from", its address, "port" and its
 * port.
 */
[[nodiscard]] std::string senderText(const rtype::network::Endpoint& sender);

/** @returns A datagram holding @p input. */
[[nodiscard]] std::vector<std::byte> inputDatagram(
    const rtype::network::PlayerInput& input,
    const rtype::network::PacketHeader& header = {});
