#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <type_traits>
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/**
 * @brief The player asks for the best scores of this server for a difficulty
 * and a number of players at the start of a game: ALL_DIFFICULTIES and
 * ALL_PLAYER_COUNTS ask for every one of them.
 */
struct LeaderboardRequest {
  static constexpr MessageType TYPE = MessageType::LeaderboardRequest;

  std::uint8_t playerCount{};
  std::string difficulty;

  bool operator==(const LeaderboardRequest&) const = default;
};

/** @returns The difficulty @p request reads; none when it asks for every one
 * (ALL_DIFFICULTIES). */
[[nodiscard]] std::optional<std::string> difficultyFilterOf(
    const LeaderboardRequest& request);

/** @returns The number of players @p request reads; none when it asks for
 * every one (ALL_PLAYER_COUNTS). */
[[nodiscard]] std::optional<std::uint8_t> playerCountFilterOf(
    const LeaderboardRequest& request);

/**
 * @brief Writes the number of players, then the difficulty.
 * @throws InvalidFieldValueException when the number is neither
 * ALL_PLAYER_COUNTS nor 1 to MAXIMUM_PLAYERS.
 * @throws InvalidDifficultyException when the difficulty is neither
 * ALL_DIFFICULTIES nor a name that keeps the rules of a nickname. Nothing is
 * written when either is raised.
 */
void encodePayload(ByteWriter& writer, const LeaderboardRequest& message);

/**
 * @brief Reads and checks the fields of a LeaderboardRequest, and leaves what
 * follows them in @p reader, for a message that starts with them.
 * @throws What decodePayload() throws, TrailingBytesException aside.
 */
[[nodiscard]] LeaderboardRequest readLeaderboardRequest(ByteReader& reader);

/**
 * @brief Reads a LeaderboardRequest from the whole of @p payload.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidFieldValueException or InvalidDifficultyException for a value
 * the protocol does not allow.
 * @throws TrailingBytesException when bytes follow the difficulty.
 */
[[nodiscard]] LeaderboardRequest decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<LeaderboardRequest> type);

}  // namespace rtype::network
