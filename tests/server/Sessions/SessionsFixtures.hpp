#pragma once

#include <cstddef>
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "MessageOrigin.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"

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

/**
 * @brief Connects the player at @p index to @p roster, under SESSION_TOKEN.
 * @returns The player added; it stays at that address until it is removed.
 */
ConnectedPlayer& addPlayer(PlayerRoster& roster, std::size_t index = 0);

}  // namespace rtype::server::testing
