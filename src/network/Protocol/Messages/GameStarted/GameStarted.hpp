#pragma once

#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Every connected player is ready: the game starts. A lost one is
 * corrected by the first WorldState. It has no payload.
 */
struct GameStarted {
  static constexpr MessageType TYPE = MessageType::GameStarted;

  bool operator==(const GameStarted&) const = default;
};

/**
 * @brief Writes nothing: the message has no payload.
 */
void encodePayload(ByteWriter& writer, const GameStarted& message);

/**
 * @brief Reads a GameStarted from @p payload, which must be empty.
 * @throws TrailingBytesException when @p payload is not empty.
 */
[[nodiscard]] GameStarted decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<GameStarted> type);

}  // namespace rtype::network
