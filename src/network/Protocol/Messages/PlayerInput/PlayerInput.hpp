#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The keys a player holds, sent 60 times per second during a game,
 * even when none is held, with the inputs before it that the server has not
 * confirmed yet. inputNumber starts at 1 and grows by 1 with each input; the
 * server keeps the highest. actions holds one bit per action from bit 0: up,
 * down, left, right, shoot. earlierActions holds the actions of the inputs
 * numbered inputNumber - 1, inputNumber - 2 and so on, newest first, at most
 * MAXIMUM_REPEATED_INPUTS of them and none below FIRST_INPUT_NUMBER.
 */
struct PlayerInput {
  static constexpr MessageType TYPE = MessageType::PlayerInput;

  std::uint32_t inputNumber{};
  std::uint8_t actions{};
  std::vector<std::uint8_t> earlierActions;

  bool operator==(const PlayerInput&) const = default;
};

/**
 * @brief Writes the input number, the actions, the number of earlier actions,
 * then the earlier actions.
 * @throws InvalidFieldValueException when a bit above PLAYER_ACTION_COUNT is
 * set in an action, when there are more than MAXIMUM_REPEATED_INPUTS earlier
 * actions, or when they reach below FIRST_INPUT_NUMBER; nothing is written
 * then.
 */
void encodePayload(ByteWriter& writer, const PlayerInput& message);

/**
 * @brief Reads a PlayerInput from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException when a bit above PLAYER_ACTION_COUNT is
 * set in an action, or the number of earlier actions is above
 * MAXIMUM_REPEATED_INPUTS or reaches below FIRST_INPUT_NUMBER.
 * @throws TrailingBytesException when bytes follow the earlier actions.
 */
[[nodiscard]] PlayerInput decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<PlayerInput> type);

}  // namespace rtype::network
