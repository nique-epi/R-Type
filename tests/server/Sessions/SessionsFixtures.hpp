#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>
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

}  // namespace rtype::server::testing
