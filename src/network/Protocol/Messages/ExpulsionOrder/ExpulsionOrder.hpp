#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The supervisor orders a game process to expel the players of that
 * nickname (see sameNickname()), under the link token of the game. Not
 * guaranteed: the supervisor sends it FAREWELL_COPIES times, and a copy that
 * finds no player of that nickname left does nothing.
 */
struct ExpulsionOrder {
  static constexpr MessageType TYPE = MessageType::ExpulsionOrder;

  /** @brief A nickname the protocol accepts (isValidNickname()). */
  std::string nickname;

  bool operator==(const ExpulsionOrder&) const = default;
};

/**
 * @brief Writes the nickname.
 * @throws InvalidNicknameException when it is not one the protocol accepts;
 * nothing is written then.
 */
void encodePayload(ByteWriter& writer, const ExpulsionOrder& message);

/**
 * @brief Reads an ExpulsionOrder from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidNicknameException when the nickname is not one the protocol
 * accepts.
 * @throws TrailingBytesException when bytes follow the nickname.
 */
[[nodiscard]] ExpulsionOrder decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<ExpulsionOrder> type);

}  // namespace rtype::network
