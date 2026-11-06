#include "GamePanel.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include "BackgroundColorConstants.hpp"
#include "CutCornerBox.hpp"
#include "GameCatalog.hpp"
#include "GameListSnapshot.hpp"
#include "GameRow.hpp"
#include "IInterfaceCanvas.hpp"
#include "InkTag.hpp"
#include "InterfaceColorConstants.hpp"
#include "ListedGameState.hpp"
#include "SectorMapConstants.hpp"
#include "ServerListPhase.hpp"
#include "ServerMapScreenConstants.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

namespace {

const char* stateWordOf(network::ListedGameState state) {
  switch (state) {
    case network::ListedGameState::Waiting:
      return WAITING_WORD;
    case network::ListedGameState::InGame:
      return IN_GAME_WORD;
    case network::ListedGameState::Full:
      break;
  }
  return FULL_WORD;
}

/** @returns The line that says what ENTER does with @p game. */
std::string descriptionOf(const GameRow& game) {
  if (isJoinable(game)) {
    return game.name + JOINABLE_DESCRIPTION;
  }
  return game.name + (game.state == network::ListedGameState::InGame
                          ? IN_GAME_DESCRIPTION
                          : FULL_DESCRIPTION);
}

sf::Vector2i inPanel(int across, int down) {
  return GAME_PANEL_AREA.position + sf::Vector2i{across, down};
}

/** @returns The words the panel shows in place of its rows, or nothing when
 * it has rows to show. */
std::optional<const char*> messageOf(ServerListPhase phase) {
  switch (phase) {
    case ServerListPhase::Searching:
      return SEARCHING_GAMES_TITLE;
    case ServerListPhase::Empty:
      return NO_GAME_TITLE;
    case ServerListPhase::MasterUnreachable:
      return MASTER_UNREACHABLE_TITLE;
    case ServerListPhase::Ready:
      break;
  }
  return std::nullopt;
}

void paintColumnHeadings(IInterfaceCanvas& canvas) {
  const TextStyle heading{.font = TextFont::Small, .color = BLUE};
  canvas.write(GAME_COLUMN_LABEL,
               inPanel(GAME_NAME_COLUMN, GAME_PANEL_COLUMNS_TOP), heading);
  canvas.write(PILOTS_COLUMN_LABEL,
               inPanel(GAME_PILOTS_COLUMN, GAME_PANEL_COLUMNS_TOP), heading);
  canvas.write(STATE_COLUMN_LABEL,
               inPanel(GAME_STATE_COLUMN, GAME_PANEL_COLUMNS_TOP), heading);
}

/** @brief Where a row of the panel lies, and whether it is the chosen one. */
struct GameRowPlace {
  int top{};
  bool chosen{};
};

/** @brief Paints @p game on its row: its name, its pilots of its seats, and
 * its state as a word on ink; the chosen row on a highlight, in ink. */
void paintGameRow(IInterfaceCanvas& canvas, const GameRow& game,
                  GameRowPlace place) {
  if (place.chosen) {
    canvas.fill(
        {inPanel(GAME_ROW_HIGHLIGHT_LEFT, place.top - GAME_ROW_HIGHLIGHT_RISE),
         {GAME_PANEL_AREA.size.x - (2 * GAME_ROW_HIGHLIGHT_LEFT),
          GAME_ROW_HIGHLIGHT_HEIGHT}},
        EDGE_BLUE);
  }
  canvas.write(game.name, inPanel(GAME_NAME_COLUMN, place.top),
               TextStyle{.font = TextFont::Regular,
                         .color = place.chosen ? INK : PALE_BLUE});
  canvas.write(std::to_string(game.playerCount) + PILOTS_SEPARATOR +
                   std::to_string(game.maximumPlayers),
               inPanel(GAME_PILOTS_COLUMN, place.top),
               TextStyle{.font = TextFont::Regular,
                         .color = place.chosen ? INK : STAR_WHITE});
  paintInkTag(canvas,
              InkTag{.word = stateWordOf(game.state),
                     .topLeft = inPanel(GAME_STATE_COLUMN, place.top),
                     .wordColor = isJoinable(game) ? HUD_CYAN : STAR_GOLD});
}

}  // namespace

void GamePanel::show(const GameListSnapshot& games, std::string serverName) {
  const std::optional<std::uint16_t> before =
      chosen_ < games_.games.size()
          ? std::optional{games_.games[chosen_].number}
          : std::nullopt;
  games_ = games;
  serverName_ = std::move(serverName);
  chosen_ = 0;
  if (before.has_value()) {
    const auto still =
        std::ranges::find(games_.games, *before, &GameRow::number);
    chosen_ = still == games_.games.end()
                  ? 0
                  : static_cast<std::size_t>(still - games_.games.begin());
  }
  move(0);
}

void GamePanel::move(int step) {
  if (games_.games.empty()) {
    chosen_ = 0;
    first_ = 0;
    return;
  }
  const auto last = static_cast<std::int64_t>(games_.games.size()) - 1;
  chosen_ = static_cast<std::size_t>(std::clamp(
      static_cast<std::int64_t>(chosen_) + step, std::int64_t{0}, last));
  if (chosen_ < first_) {
    first_ = chosen_;
  } else if (chosen_ >= first_ + GAME_ROWS_SHOWN) {
    first_ = chosen_ + 1 - GAME_ROWS_SHOWN;
  }
}

std::optional<GameRow> GamePanel::joinable() const {
  if (chosen_ >= games_.games.size() || !isJoinable(games_.games[chosen_])) {
    return std::nullopt;
  }
  return games_.games[chosen_];
}

void GamePanel::paint(IInterfaceCanvas& canvas) const {
  canvas.fill(SECTOR_MAP_AREA, DIM_LAYER_COLOR);
  CutCornerBox(GAME_PANEL_AREA, {.border = HUD_CYAN, .inside = SKY_COLOR})
      .paint(canvas);
  canvas.write(serverName_, inPanel(GAME_PANEL_PADDING, GAME_PANEL_TITLE_TOP),
               TextStyle{.font = TextFont::Regular, .color = HUD_CYAN});
  canvas.fill({inPanel(GAME_PANEL_PADDING, GAME_PANEL_RULE_TOP),
               {GAME_PANEL_AREA.size.x - (2 * GAME_PANEL_PADDING), 1}},
              DEEP_BLUE);
  if (const std::optional<const char*> message = messageOf(games_.phase)) {
    canvas.write(*message,
                 inPanel(GAME_PANEL_AREA.size.x / 2, GAME_PANEL_FIRST_ROW),
                 TextStyle{.font = TextFont::Regular,
                           .color = PALE_BLUE,
                           .alignment = TextAlignment::Center});
    return;
  }
  paintRows(canvas);
  paintDescription(canvas);
}

void GamePanel::paintRows(IInterfaceCanvas& canvas) const {
  paintColumnHeadings(canvas);
  const std::size_t last =
      std::min(games_.games.size(), first_ + GAME_ROWS_SHOWN);
  for (std::size_t index = first_; index < last; ++index) {
    paintGameRow(
        canvas, games_.games[index],
        GameRowPlace{.top = GAME_PANEL_FIRST_ROW +
                            (static_cast<int>(index - first_) * GAME_ROW_PITCH),
                     .chosen = index == chosen_});
  }
}

void GamePanel::paintDescription(IInterfaceCanvas& canvas) const {
  if (chosen_ >= games_.games.size()) {
    return;
  }
  const GameRow& game = games_.games[chosen_];
  canvas.write(descriptionOf(game),
               inPanel(GAME_PANEL_PADDING, GAME_PANEL_DESCRIPTION_TOP),
               TextStyle{.font = TextFont::Small,
                         .color = isJoinable(game) ? HUD_CYAN : PALE_BLUE});
}

}  // namespace rtype::client
