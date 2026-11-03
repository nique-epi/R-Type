#include "LeaderboardPresentation.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ItemCell.hpp"
#include "ItemRow.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardMessage.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardRequest.hpp"
#include "LeaderboardScreenConstants.hpp"
#include "ScreenConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

std::string twoDigits(unsigned int value) {
  return (value < FIRST_NUMBER_OF_TWO_DIGITS ? "0" : "") +
         std::to_string(value);
}

sf::Color rankColorOf(std::uint32_t rank) {
  if (rank >= 1 && rank <= LEADERBOARD_TOP_RANK_COLORS.size()) {
    return LEADERBOARD_TOP_RANK_COLORS.at(rank - 1);
  }
  return LEADERBOARD_RANK_COLOR;
}

ItemRow rowOf(const network::LeaderboardRow& entry, bool isOwn) {
  const sf::Color text =
      isOwn ? LEADERBOARD_OWN_TEXT_COLOR : LEADERBOARD_TEXT_COLOR;
  return ItemRow{
      .cells = {ItemCell{.text = std::to_string(entry.rank),
                         .color = isOwn ? text : rankColorOf(entry.rank)},
                ItemCell{
                    .text = entry.nickname,
                    .color = text,
                    .tag = isOwn ? std::optional<std::string>(LOCAL_PLAYER_TAG)
                                 : std::nullopt},
                ItemCell{.text = std::to_string(entry.score), .color = text},
                ItemCell{.text = dateTextOf(entry.playedAt),
                         .color = isOwn ? LEADERBOARD_OWN_DATE_COLOR
                                        : LEADERBOARD_DATE_COLOR}},
      .highlight = isOwn ? std::optional<sf::Color>(LEADERBOARD_OWN_ROW_COLOR)
                         : std::nullopt};
}

}  // namespace

std::string dateTextOf(std::int64_t secondsSince1970) {
  if (secondsSince1970 < 0 || secondsSince1970 > LAST_SHOWN_DATE_SECONDS) {
    return UNKNOWN_DATE_TEXT;
  }
  const std::chrono::sys_seconds moment{std::chrono::seconds{secondsSince1970}};
  const std::chrono::year_month_day date{
      std::chrono::floor<std::chrono::sys_days::duration>(moment)};
  const auto month =
      static_cast<std::size_t>(static_cast<unsigned int>(date.month()) - 1);
  const auto year = static_cast<unsigned int>(static_cast<int>(date.year()) %
                                              YEARS_PER_CENTURY);
  return twoDigits(static_cast<unsigned int>(date.day())) + " " +
         MONTH_NAMES.at(month) + " " + twoDigits(year);
}

std::vector<ItemRow> itemRowsOf(const LeaderboardPage& page) {
  std::vector<ItemRow> rows;
  rows.reserve(page.rows.size());
  for (const network::LeaderboardRow& entry : page.rows) {
    rows.push_back(rowOf(entry, page.ownRow == entry));
  }
  return rows;
}

std::optional<ItemRow> pinnedRowOf(const LeaderboardPage& page) {
  if (!page.ownRow.has_value() ||
      std::ranges::find(page.rows, *page.ownRow) != page.rows.end()) {
    return std::nullopt;
  }
  return rowOf(*page.ownRow, true);
}

std::string pinnedNoteOf(const LeaderboardPage& page) {
  return page.rows.empty() || page.ownRow.has_value() ? "" : NO_OWN_SCORE_TEXT;
}

std::optional<LeaderboardMessage> messageOf(const LeaderboardPage& page) {
  switch (page.phase) {
    case LeaderboardPhase::Answered:
      return page.rows.empty() ? std::optional(EMPTY_LEADERBOARD_MESSAGE)
                               : std::nullopt;
    case LeaderboardPhase::NoAnswer:
      return NO_ANSWER_MESSAGE;
    case LeaderboardPhase::NotOnServer:
      return NOT_ON_SERVER_MESSAGE;
    case LeaderboardPhase::OutOfReach:
      return OUT_OF_REACH_MESSAGE;
    case LeaderboardPhase::Asking:
      break;
  }
  return std::nullopt;
}

bool canBeFiltered(const LeaderboardPage& page) {
  return page.phase != LeaderboardPhase::NotOnServer &&
         page.phase != LeaderboardPhase::OutOfReach;
}

LeaderboardFilterChoice openingChoiceOf(std::size_t listedPilots) {
  return LeaderboardFilterChoice{
      .difficulty = DEFAULT_DIFFICULTY_CHOICE,
      .pilots = std::clamp<std::size_t>(listedPilots, 1,
                                        LEADERBOARD_PILOT_LABELS.size()) -
                1};
}

network::LeaderboardRequest filterOf(LeaderboardFilterChoice choice) {
  return network::LeaderboardRequest{
      .playerCount = static_cast<std::uint8_t>(choice.pilots + 1),
      .difficulty =
          std::string(LEADERBOARD_DIFFICULTY_NAMES.at(choice.difficulty))};
}

std::string filterSummaryOf(LeaderboardFilterChoice choice) {
  return std::string(LEADERBOARD_DIFFICULTY_LABELS.at(choice.difficulty)) +
         SUMMARY_GAMES_TEXT + LEADERBOARD_PILOT_LABELS.at(choice.pilots) +
         (choice.pilots == 0 ? ONE_PILOT_TEXT : SEVERAL_PILOTS_TEXT);
}

}  // namespace rtype::client
