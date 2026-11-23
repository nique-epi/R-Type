#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <type_traits>
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief Every connected player is ready: the game starts. The server repeats
 * it to the players still loading, and a lost one is corrected by the first
 * WorldState.
 *
 * It tells the sky the level opens in, so a client shows that biome and plays
 * its music while the game loads, before the first LevelStatus: the biome,
 * empty for a level that names none yet, otherwise a biome name (see
 * identifiers::isBiomeName()); the scroll position, in logical units, finite
 * and never negative; and the position of the scroll when that biome began,
 * finite, negative for a biome the level opens in. The scroll does not move
 * before the game is in progress. These follow the rules of the same fields
 * of LevelStatus, which then takes over.
 */
struct GameStarted {
  static constexpr MessageType TYPE = MessageType::GameStarted;

  std::string biome;
  float scrollPosition{};
  float biomeSince{};

  bool operator==(const GameStarted&) const = default;
};

/**
 * @brief Writes the biome, as a short string, then the scroll position and the
 * position the biome began at.
 * @throws InvalidFieldValueException when the message breaks a rule of
 * GameStarted; nothing is written then.
 */
void encodePayload(ByteWriter& writer, const GameStarted& message);

/**
 * @brief Reads a GameStarted from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload ends before the position
 * the biome began at.
 * @throws TrailingBytesException when bytes follow it.
 * @throws InvalidFieldValueException when the message breaks a rule of
 * GameStarted.
 */
[[nodiscard]] GameStarted decodePayload(std::span<const std::byte> payload,
                                        std::type_identity<GameStarted> type);

}  // namespace rtype::network
