#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <type_traits>
#include <vector>
#include "ByteWriter.hpp"
#include "LeaderboardRequest.hpp"
#include "MessageType.hpp"

namespace rtype::network {

/** @brief The best score of one player on a leaderboard. */
struct LeaderboardRow {
  /** @brief 1 for the best score; equal scores share a rank. */
  std::uint32_t rank{};
  std::uint32_t score{};
  /** @brief When the game ended, in seconds since 1970 UTC. */
  std::int64_t playedAt{};
  std::string nickname;

  bool operator==(const LeaderboardRow&) const = default;
};

/**
 * @brief The best scores of a leaderboard, and the row of the player who asked
 * when that player has one, whether or not it is among them.
 */
struct LeaderboardAnswer {
  static constexpr MessageType TYPE = MessageType::LeaderboardAnswer;

  /** @brief The leaderboard this answer lists. */
  LeaderboardRequest asked;
  /** @brief At most MAXIMUM_LEADERBOARD_ROWS rows, the best first. */
  std::vector<LeaderboardRow> rows;
  std::optional<LeaderboardRow> ownRow;

  bool operator==(const LeaderboardAnswer&) const = default;
};

/**
 * @brief Writes the leaderboard asked for, the count of rows, each row, then
 * whether the player has a row of its own and that row.
 * @throws TooManyLeaderboardRowsException when there are more than
 * MAXIMUM_LEADERBOARD_ROWS rows.
 * @throws InvalidFieldValueException when a rank is 0, or for the fields of the
 * leaderboard asked for.
 * @throws InvalidNicknameException for a nickname the protocol refuses.
 * @throws InvalidDifficultyException for the difficulty.
 * Nothing is written when any of them is raised.
 */
void encodePayload(ByteWriter& writer, const LeaderboardAnswer& message);

/**
 * @brief Reads a LeaderboardAnswer from the whole of @p payload. The count of
 * rows is checked before any row is read.
 * @throws TooManyLeaderboardRowsException when it announces more than
 * MAXIMUM_LEADERBOARD_ROWS.
 * @throws BufferUnderflowException when @p payload is cut short.
 * @throws InvalidBoolException when the flag of the own row is not a bool.
 * @throws What encodePayload() throws for a value the protocol refuses.
 * @throws TrailingBytesException when bytes follow the last field.
 */
[[nodiscard]] LeaderboardAnswer decodePayload(
    std::span<const std::byte> payload,
    std::type_identity<LeaderboardAnswer> type);

}  // namespace rtype::network
