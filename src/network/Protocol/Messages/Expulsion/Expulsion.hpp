#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Why a game expels a player. The values are the ones of the protocol
 * page; any other byte is invalid.
 */
enum class ExpulsionReason : std::uint8_t {
  /** @brief The host of the server typed kick in its console. */
  ByTheHost = 1,
  /** @brief The host banned the player. */
  Banned = 2,
};

/**
 * @brief The game tells a player, in its session, that it is expelled; the
 * session closes at the next tick. Not guaranteed: sent FAREWELL_COPIES times
 * in a row, since nothing follows it.
 */
struct Expulsion {
  static constexpr MessageType TYPE = MessageType::Expulsion;

  ExpulsionReason reason{ExpulsionReason::ByTheHost};

  bool operator==(const Expulsion&) const = default;
};

/**
 * @brief Writes the reason.
 * @throws InvalidFieldValueException when it is not one of ExpulsionReason;
 * nothing is written then.
 */
void encodePayload(ByteWriter& writer, const Expulsion& message);

/**
 * @brief Reads an Expulsion from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is empty.
 * @throws InvalidFieldValueException when the reason is not one of
 * ExpulsionReason.
 * @throws TrailingBytesException when bytes follow the reason.
 */
[[nodiscard]] Expulsion decodePayload(std::span<const std::byte> payload,
                                      std::type_identity<Expulsion> type);

}  // namespace rtype::network
