#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ItemRow.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardMessage.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardRequest.hpp"

namespace rtype::client {

/** @returns A date in UTC as a row writes it, such as 18 OCT 26;
 * UNKNOWN_DATE_TEXT for a date before 1970 or past LAST_SHOWN_DATE_SECONDS,
 * which only a broken server sends. */
[[nodiscard]] std::string dateTextOf(std::int64_t secondsSince1970);

/** @returns A row per entry of the page, the entry of the player highlighted.
 */
[[nodiscard]] std::vector<ItemRow> itemRowsOf(const LeaderboardPage& page);

/** @returns The row of the player when it is not among the entries; nothing
 * when it is, or when the player has none. */
[[nodiscard]] std::optional<ItemRow> pinnedRowOf(const LeaderboardPage& page);

/** @returns What the panel says in place of rows: nothing while it shows rows
 * or asks. */
[[nodiscard]] std::optional<LeaderboardMessage> messageOf(
    const LeaderboardPage& page);

/** @returns Whether the player can filter what the page shows: a source is
 * there to ask. */
[[nodiscard]] bool canBeFiltered(const LeaderboardPage& page);

/** @returns The leaderboard a choice of the player asks for. */
[[nodiscard]] network::LeaderboardRequest filterOf(
    LeaderboardFilterChoice choice);

/** @returns The line that says which leaderboard is read, such as NORMAL GAMES,
 * 2 PILOTS. */
[[nodiscard]] std::string filterSummaryOf(LeaderboardFilterChoice choice);

}  // namespace rtype::client
