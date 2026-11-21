#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief What the players see of the level a game plays: its biome and its
 * scroll. Guaranteed and ordered: the server sends one when the level starts,
 * when its biome changes, when its scroll stops or starts again, and when the
 * level goes back to a checkpoint. Each holds the whole state and replaces
 * the one handed over before it.
 *
 * The biome is empty before the level names one, otherwise a biome name (see
 * identifiers::isBiomeName()). The scroll position, in logical units, the
 * scroll speed, in logical units per second, 0 while the scroll is paused,
 * are finite and never negative; the position of the scroll when the biome
 * began, where its decor starts sliding in, is finite, and negative for a
 * biome a level opens in, which began before its scroll. The time left of the
 * transition the level plays between two of its parts, in milliseconds, is 0
 * when it plays none: its players are held while it lasts, and a client plays
 * the cinematic of the transition from it. The band is the rows the ships fly
 * between in that biome, in logical units from the top of the playfield: its
 * top lies above its bottom, and a bottom of OPEN_BAND_BOTTOM closes nothing,
 * the ships flying down to the bottom of the play area.
 */
struct LevelStatus {
  static constexpr MessageType TYPE = MessageType::LevelStatus;

  std::string biome;
  float scrollPosition{};
  float scrollSpeed{};
  float biomeSince{};
  std::uint16_t transitionLeft{};
  std::uint16_t bandTop{};
  std::uint16_t bandBottom{OPEN_BAND_BOTTOM};
  std::uint8_t continuesLeft{};

  bool operator==(const LevelStatus&) const = default;
};

/**
 * @brief Writes the biome, as a short string, then the scroll position, the
 * scroll speed, the position the biome began at, the time left of the
 * transition and the top and the bottom of the band.
 * @throws InvalidFieldValueException when the status breaks a rule of
 * LevelStatus; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const LevelStatus& message);

/**
 * @brief Reads a LevelStatus from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload ends before the bottom of
 * the band.
 * @throws TrailingBytesException when bytes follow that bottom.
 * @throws InvalidFieldValueException when the status breaks a rule of
 * LevelStatus.
 */
[[nodiscard]] LevelStatus decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<LevelStatus> type);

}  // namespace rtype::network
