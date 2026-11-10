#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief What the players see of the level a game plays: its biome and its
 * scroll. Guaranteed and ordered: the server sends one when the level starts,
 * when its biome changes, when its scroll stops or starts again, and when the
 * level goes back to a checkpoint. Each holds the whole state and replaces
 * the one handed over before it.
 *
 * The biome is empty before the level names one, otherwise a biome name (see
 * identifiers::isBiomeName()). The scroll position, in logical units, and the
 * scroll speed, in logical units per second, 0 while the scroll is paused,
 * are finite and never negative.
 */
struct LevelStatus {
  static constexpr MessageType TYPE = MessageType::LevelStatus;

  std::string biome;
  float scrollPosition{};
  float scrollSpeed{};

  bool operator==(const LevelStatus&) const = default;
};

/**
 * @brief Writes the biome, as a short string, then the scroll position and
 * the scroll speed.
 * @throws InvalidFieldValueException when the status breaks a rule of
 * LevelStatus; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const LevelStatus& message);

/**
 * @brief Reads a LevelStatus from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload ends before the speed.
 * @throws TrailingBytesException when bytes follow the speed.
 * @throws InvalidFieldValueException when the status breaks a rule of
 * LevelStatus.
 */
[[nodiscard]] LevelStatus decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<LevelStatus> type);

}  // namespace rtype::network
