#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "GameLaunch.hpp"
#include "GameLaunchOptions.hpp"
#include "GameLaunchSettings.hpp"
#include "GameLifecycle.hpp"
#include "GameOpening.hpp"
#include "GameRegistry.hpp"
#include "GameRules.hpp"
#include "GameStatus.hpp"
#include "LaunchOptions.hpp"
#include "ProcessCommand.hpp"
#include "RecordingChildState.hpp"
#include "SeatRequest.hpp"
#include "SupervisedGame.hpp"

/** @returns The port game @p number gets in the registry tests. */
[[nodiscard]] std::uint16_t portOfGame(std::uint32_t number);

/** @returns The link token game @p number gets in the registry tests. */
[[nodiscard]] std::uint64_t tokenOfGame(std::uint32_t number);

/** @returns The opening of game @p number, whose process shares @p state with
 * the test, opened for a NewGame of @p creator when there is one. */
[[nodiscard]] rtype::server::GameOpening openingOf(
    std::uint32_t number, const std::shared_ptr<RecordingChildState>& state,
    std::optional<rtype::network::Endpoint> creator = std::nullopt);

/** @brief Opens game @p number in @p registry, then takes @p status as its
 * status of sequence SEQUENCE_FIVE. */
void openReporting(rtype::server::GameRegistry& registry, std::uint32_t number,
                   const rtype::network::GameStatus& status);

/** @returns The choice of the game in @p slot. */
[[nodiscard]] rtype::network::GameChoice choiceOfGame(std::uint16_t slot);

/** @returns A status of @p state with the first @p playerCount nicknames of
 * SUPERVISED_NICKNAMES, at most four, and no measure. */
[[nodiscard]] rtype::network::GameStatus statusWithPlayers(
    rtype::network::GameStatusState state, std::size_t playerCount);

/** @returns A game of the registry in @p slot and @p lifecycle, with the
 * first @p playerCount nicknames of SUPERVISED_NICKNAMES and no measure. */
[[nodiscard]] rtype::server::SupervisedGame supervisedGameIn(
    std::uint16_t slot, rtype::server::GameLifecycle lifecycle,
    std::size_t playerCount);

/** @returns The argument that follows @p option in @p command; empty when the
 * option is not there. */
[[nodiscard]] std::string argumentAfter(
    const rtype::server::ProcessCommand& command, std::string_view option);

/** @returns Where the player at @p index sends from in the supervisor tests. */
[[nodiscard]] rtype::network::Endpoint supervisedPlayer(std::size_t index);

/** @returns The request of the player at @p index: it sends from
 * supervisedPlayer(@p index) and asks for PILOT<index + 1>, so the first four
 * are the nicknames of SUPERVISED_NICKNAMES. */
[[nodiscard]] rtype::server::SeatRequest seatRequestOf(std::size_t index);

/** @returns The rules a creator chose in the tests: CHOSEN_DIFFICULTY,
 * CHOSEN_LIVES and friendly fire on. */
[[nodiscard]] rtype::network::GameRules chosenRules();

/** @returns Where the game on @p port listens, on the loopback. */
[[nodiscard]] rtype::network::Endpoint loopbackOn(std::uint16_t port);

/** @returns What the games of the supervisor of @p supervisor are started
 * with, in FOLDER_FOR_TESTS: its forwarded options and its traffic file. */
[[nodiscard]] rtype::server::GameLaunchSettings settingsOf(
    const rtype::server::LaunchOptions& supervisor);

/** @returns The launch of game 3 on FIRST_GAME_PORT. */
[[nodiscard]] rtype::server::GameLaunch launchOfGameThree();

/** @returns The options a game process reads from @p command, the arguments
 * after `--mode game`. */
[[nodiscard]] rtype::server::GameLaunchOptions gameOptionsOf(
    const rtype::server::ProcessCommand& command);
