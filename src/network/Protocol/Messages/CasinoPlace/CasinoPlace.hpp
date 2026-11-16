#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "TablePlace.hpp"

namespace rtype::network {

/**
 * @brief The place the player chose: the client sends it when the player sits
 * at the roulette table or leaves it, and again until a RouletteRound shows
 * it. An absolute value, never a change.
 */
struct CasinoPlace {
  static constexpr MessageType TYPE = MessageType::CasinoPlace;

  TablePlace place{TablePlace::InRoom};

  bool operator==(const CasinoPlace&) const = default;
};

/** @brief Writes the place in one byte.
 * @throws InvalidFieldValueException when it is not one of TablePlace. */
void encodePayload(ByteWriter& writer, const CasinoPlace& message);

/**
 * @brief Reads a CasinoPlace from the whole of @p payload.
 * @throws BufferUnderflowException when it is empty.
 * @throws InvalidFieldValueException naming place when the byte is not one of
 * TablePlace.
 * @throws TrailingBytesException when bytes follow it.
 */
[[nodiscard]] CasinoPlace decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<CasinoPlace> type);

}  // namespace rtype::network
