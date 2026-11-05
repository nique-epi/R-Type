#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "MessageOrigin.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

/**
 * @returns Where a message comes from when the player at @p index sends it in
 * the datagram of sequence @p sequence.
 */
[[nodiscard]] network::MessageOrigin originOf(std::size_t index,
                                              std::uint16_t sequence = 0);

/** @returns The input numbered @p inputNumber that holds @p actions. */
[[nodiscard]] network::PlayerInput inputNumbered(std::uint32_t inputNumber,
                                                 std::uint8_t actions);

/** @returns The input numbered @p inputNumber that holds @p actions and repeats
 * the inputs before it, whose actions are @p earlierActions, newest first. */
[[nodiscard]] network::PlayerInput inputRepeating(
    std::uint32_t inputNumber, std::uint8_t actions,
    std::vector<std::uint8_t> earlierActions);

/**
 * @brief Connects the player at @p index to @p roster, under SESSION_TOKEN.
 * @returns The player added; it stays at that address until it is removed.
 */
ConnectedPlayer& addPlayer(PlayerRoster& roster, std::size_t index = 0);

/** @brief Connects the players at indexes 0 to @p count - 1 to @p roster, as
 * addPlayer() does. */
void addPlayers(PlayerRoster& roster, std::size_t count);

/** @brief What an InputKeeper showed the game: an input, and the input number
 * the player kept when it was shown. */
struct ShownInput {
  network::PlayerInput input{};
  std::optional<std::uint32_t> numberKeptBefore;
};

/** @brief Reads nothing of the inputs it is shown: the onNewerInput of a test
 * that does not look at them. */
void ignoreInput(const ConnectedPlayer& player,
                 const network::PlayerInput& input);

/** @returns An onNewerInput that adds what it is shown to @p shown, which
 * must outlive it. */
[[nodiscard]] std::function<void(const ConnectedPlayer&,
                                 const network::PlayerInput&)>
recorderOf(std::vector<ShownInput>& shown);

/** @returns The datagrams sendHeartbeats() sends the players of @p roster at
 * @p now, in order. */
[[nodiscard]] std::vector<network::OutgoingDatagram> heartbeatsSentAt(
    PlayerRoster& roster, engine::Duration now);

/**
 * @brief Hands @p roster a Heartbeat from @p sender that echoes the Heartbeat
 * the server sent at HEARTBEAT_SENDING_TIME, held HELD_BY_THE_CLIENT, in a
 * datagram that arrived at ECHO_ARRIVAL: a round trip of
 * MEASURED_PLAYER_ROUND_TRIP.
 */
void readEchoFrom(PlayerRoster& roster, const network::Endpoint& sender);

}  // namespace rtype::server::testing
