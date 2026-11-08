#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "GameRules.hpp"
#include "Heartbeat.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "ServerLobbyTestConstants.hpp"

/** @returns A datagram holding a ConnectionRequest without a cookie. */
[[nodiscard]] std::vector<std::byte> requestDatagram(
    const rtype::network::PacketHeader& header = {});

/** @returns A datagram holding a ConnectionRequest that carries @p cookie. */
[[nodiscard]] std::vector<std::byte> requestDatagram(
    std::uint64_t cookie, const rtype::network::PacketHeader& header = {});

/** @returns A datagram holding a ConnectionRequest that carries @p cookie and
 * asks for @p game, as PLAYER_NICKNAME, with @p rules; without them, with the
 * rules a client sends for @p game when its player chose none. */
[[nodiscard]] std::vector<std::byte> requestDatagramFor(
    const rtype::network::GameChoice& game, std::uint64_t cookie,
    const std::optional<rtype::network::GameRules>& rules = std::nullopt);

/** @returns A datagram holding a ConnectionRequest that carries @p cookie and
 * asks for @p nickname. */
[[nodiscard]] std::vector<std::byte> requestDatagramOf(
    const std::string& nickname, std::uint64_t cookie);

/** @returns A datagram holding a ServerInformationRequest sent at
 * @p sentAt. */
[[nodiscard]] std::vector<std::byte> informationRequestDatagram(
    const rtype::network::PacketHeader& header = {}, std::uint32_t sentAt = 0);

/** @returns A datagram holding a ConnectionRequest without a cookie, then a
 * ServerInformationRequest. */
[[nodiscard]] std::vector<std::byte> requestAndInformationRequestDatagram();

/** @returns A datagram holding a probe of the master with @p header; the
 * sorter reads no payload, so its secret and token are 0. */
[[nodiscard]] std::vector<std::byte> probeDatagram(
    const rtype::network::PacketHeader& header = {});

/** @returns A datagram holding a ServerInformationRequest, then a probe of the
 * master. */
[[nodiscard]] std::vector<std::byte> informationRequestAndProbeDatagram();

/** @returns A datagram holding a ReadyState, which only a player sends. */
[[nodiscard]] std::vector<std::byte> readyDatagram(
    const rtype::network::PacketHeader& header = {});

/** @returns A header carrying @p sessionToken, as an admitted player sends. */
[[nodiscard]] rtype::network::PacketHeader playerHeader(
    std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);

/** @returns A ReadyState carrying @p sessionToken, as an admitted player
 * sends. */
[[nodiscard]] std::vector<std::byte> playerReady(
    std::uint64_t sessionToken = rtype::server::testing::SESSION_TOKEN);

/**
 * @returns How a log line names @p sender: "from", its address, "port" and its
 * port.
 */
[[nodiscard]] std::string senderText(const rtype::network::Endpoint& sender);

/** @returns A datagram holding a Heartbeat, which says only that its sender
 * is still there. */
[[nodiscard]] std::vector<std::byte> heartbeatDatagram(
    const rtype::network::PacketHeader& header = {});

/** @returns A datagram holding @p heartbeat, which a client sends with the echo
 * of the newest Heartbeat of the server. */
[[nodiscard]] std::vector<std::byte> heartbeatDatagram(
    const rtype::network::PacketHeader& header,
    const rtype::network::Heartbeat& heartbeat);

/** @returns A datagram holding a Disconnection, which a player sends when it
 * leaves. */
[[nodiscard]] std::vector<std::byte> disconnectionDatagram(
    const rtype::network::PacketHeader& header = {});

/** @returns A datagram holding @p input. */
[[nodiscard]] std::vector<std::byte> inputDatagram(
    const rtype::network::PlayerInput& input,
    const rtype::network::PacketHeader& header = {});
