#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The keys a player holds, sent 60 times per second during a game,
 * even when none is held. inputNumber starts at 1 and grows by 1 with each
 * input; the server keeps the highest. actions holds one bit per action from
 * bit 0: up, down, left, right, shoot.
 */
struct PlayerInput {
  static constexpr MessageType TYPE = MessageType::PlayerInput;

  std::uint32_t inputNumber;
  std::uint8_t actions;

  bool operator==(const PlayerInput&) const = default;
};

/**
 * @brief Writes the input number, then the actions.
 * @throws InvalidFieldValueException when a bit above PLAYER_ACTION_COUNT is
 * set in actions; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const PlayerInput& message);

/**
 * @brief Reads a PlayerInput from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException when a bit above PLAYER_ACTION_COUNT is
 * set in actions.
 * @throws TrailingBytesException when bytes follow the actions.
 */
[[nodiscard]] PlayerInput decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<PlayerInput> type);

}  // namespace rtype::network
