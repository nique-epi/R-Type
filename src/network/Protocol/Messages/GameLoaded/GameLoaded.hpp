#pragma once

#include <cstddef>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief A client shows the game screen: it has loaded the game and can play.
 * The server starts the game once every connected player sent one, or after
 * START_BARRIER_TICKS ticks. The client repeats it until the first WorldState
 * arrives, so a lost copy is corrected by the next. It has no payload.
 */
struct GameLoaded {
  static constexpr MessageType TYPE = MessageType::GameLoaded;

  bool operator==(const GameLoaded&) const = default;
};

/**
 * @brief Writes nothing: the message has no payload.
 */
void encodePayload(ByteWriter& writer, const GameLoaded& message);

/**
 * @brief Reads a GameLoaded from @p payload, which must be empty.
 * @throws TrailingBytesException when @p payload is not empty.
 */
[[nodiscard]] GameLoaded decodePayload(std::span<const std::byte> payload,
                                       std::type_identity<GameLoaded> type);

}  // namespace rtype::network
