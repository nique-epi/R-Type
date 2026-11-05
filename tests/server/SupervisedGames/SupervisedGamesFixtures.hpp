#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include "Endpoint.hpp"
#include "GameOpening.hpp"
#include "GameSpawnerParts.hpp"
#include "ProcessCommand.hpp"
#include "RecordingChildState.hpp"
#include "RecordingProcessLauncher.hpp"

/** @returns The port game @p number gets in the registry tests. */
[[nodiscard]] std::uint16_t portOfGame(std::uint32_t number);

/** @returns The link token game @p number gets in the registry tests. */
[[nodiscard]] std::uint64_t tokenOfGame(std::uint32_t number);

/** @returns The opening of game @p number, whose process shares @p state with
 * the test. */
[[nodiscard]] rtype::server::GameOpening openingOf(
    std::uint32_t number, const std::shared_ptr<RecordingChildState>& state);

/** @returns The argument that follows @p option in @p command; empty when the
 * option is not there. */
[[nodiscard]] std::string argumentAfter(
    const rtype::server::ProcessCommand& command, std::string_view option);

/** @returns Where the player at @p index sends from in the supervisor tests. */
[[nodiscard]] rtype::network::Endpoint supervisedPlayer(std::size_t index);

/** @returns Where the game on @p port listens, on the loopback. */
[[nodiscard]] rtype::network::Endpoint loopbackOn(std::uint16_t port);
