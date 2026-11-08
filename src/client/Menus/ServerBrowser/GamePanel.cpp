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
#include "GameRulesLine.hpp"
#include "GameRulesLineConstants.hpp"
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

/** @returns The line that says what ENTER does with @p game: a waiting game
 * names its rules before ENTER joins it. */
std::string descriptionOf(const GameRow& game) {
  if (isJoinable(game)) {
    return game.name + RULES_SEPARATOR + rulesLineOf(game.rules) +
           JOINABLE_DESCRIPTION;
  }
  return game.name + (game.state == network::ListedGameState::InGame
                          ? IN_GAME_DESCRIPTION
                          : FULL_DESCRIPTION);
}

/** @returns The rows + NEW GAME takes before the games of @p games: one once
 * the master listed them, even none, since it then listed the difficulties of
 * the server; none before. */
std::size_t newGameRowsOf(const GameListSnapshot& games) {
  return games.phase == ServerListPhase::Ready ||
                 games.phase == ServerListPhase::Empty
             ? 1
             : 0;
}

sf::Vector2i inPanel(int across, int down) {
  return GAME_PANEL_AREA.position + sf::Vector2i{across, down};
}

/** @returns Where the text of the row @p shown rows under the first one shown
 * stands, down from the top of the panel. */
int rowTopOf(std::size_t shown) {
  return GAME_PANEL_FIRST_ROW + (static_cast<int>(shown) * GAME_ROW_PITCH);
}

/** @returns The band of the row @p shown rows under the first one shown, which
 * the pointer chooses it in: as wide as its highlight, and as tall as the
 * pitch of the rows, so no click falls between two rows. */
sf::IntRect rowBandOf(std::size_t shown) {
  return {
      inPanel(GAME_ROW_HIGHLIGHT_LEFT,
              rowTopOf(shown) - GAME_ROW_HIGHLIGHT_RISE),
      {GAME_PANEL_AREA.size.x - (2 * GAME_ROW_HIGHLIGHT_LEFT), GAME_ROW_PITCH}};
}

/** @returns The words the panel shows in place of its games, or nothing when
 * it has games to show. */
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

/** @brief Paints the highlight of the chosen row, whose text stands at
 * @p top. */
void paintRowHighlight(IInterfaceCanvas& canvas, int top) {
  canvas.fill({inPanel(GAME_ROW_HIGHLIGHT_LEFT, top - GAME_ROW_HIGHLIGHT_RISE),
               {GAME_PANEL_AREA.size.x - (2 * GAME_ROW_HIGHLIGHT_LEFT),
                GAME_ROW_HIGHLIGHT_HEIGHT}},
              EDGE_BLUE);
}

/** @brief Paints @p game on its row: its name, its pilots of its seats, and
 * its state as a word on ink; the chosen row on a highlight, in ink. */
void paintGameRow(IInterfaceCanvas& canvas, const GameRow& game,
                  GameRowPlace place) {
  if (place.chosen) {
    paintRowHighlight(canvas, place.top);
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

/** @brief Paints + NEW GAME on its row, in cyan, or in ink on the highlight
 * when it is chosen. */
void paintNewGameRow(IInterfaceCanvas& canvas, GameRowPlace place) {
  if (place.chosen) {
    paintRowHighlight(canvas, place.top);
  }
  canvas.write(NEW_GAME_ROW_LABEL, inPanel(GAME_NAME_COLUMN, place.top),
               TextStyle{.font = TextFont::Regular,
                         .color = place.chosen ? INK : HUD_CYAN});
}

}  // namespace

void GamePanel::show(const GameListSnapshot& games, std::string serverName) {
  const GameRow* chosenGame = gameAt(chosen_);
  const std::optional<std::uint16_t> before =
      chosenGame != nullptr ? std::optional{chosenGame->number} : std::nullopt;
  games_ = games;
  serverName_ = std::move(serverName);
  chosen_ = 0;
  if (before.has_value()) {
    const auto still =
        std::ranges::find(games_.games, *before, &GameRow::number);
    if (still != games_.games.end()) {
      chosen_ = static_cast<std::size_t>(still - games_.games.begin()) +
                newGameRowsOf(games_);
    }
  }
  move(0);
}

void GamePanel::move(int step) {
  if (entryCount() == 0) {
    chosen_ = 0;
    first_ = 0;
    return;
  }
  const auto last = static_cast<std::int64_t>(entryCount()) - 1;
  chosen_ = static_cast<std::size_t>(std::clamp(
      static_cast<std::int64_t>(chosen_) + step, std::int64_t{0}, last));
  if (chosen_ < first_) {
    first_ = chosen_;
  } else if (chosen_ >= first_ + GAME_ROWS_SHOWN) {
    first_ = chosen_ + 1 - GAME_ROWS_SHOWN;
  }
}

std::optional<GameRow> GamePanel::joinable() const {
  const GameRow* game = gameAt(chosen_);
  if (game == nullptr || !isJoinable(*game)) {
    return std::nullopt;
  }
  return *game;
}

bool GamePanel::chooseAt(sf::Vector2i pixel) {
  const std::size_t last = std::min(entryCount(), first_ + GAME_ROWS_SHOWN);
  for (std::size_t entry = first_; entry < last; ++entry) {
    if (rowBandOf(entry - first_).contains(pixel)) {
      chosen_ = entry;
      return true;
    }
  }
  return false;
}

bool GamePanel::newGameChosen() const {
  return newGameRowsOf(games_) > 0 && chosen_ == 0;
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
  const bool offersNewGame = newGameRowsOf(games_) > 0;
  if (offersNewGame) {
    paintRows(canvas);
    paintDescription(canvas);
  }
  if (const std::optional<const char*> message = messageOf(games_.phase)) {
    canvas.write(*message,
                 inPanel(GAME_PANEL_AREA.size.x / 2,
                         offersNewGame ? GAME_PANEL_EMPTY_MESSAGE_ROW
                                       : GAME_PANEL_FIRST_ROW),
                 TextStyle{.font = TextFont::Regular,
                           .color = PALE_BLUE,
                           .alignment = TextAlignment::Center});
  }
}

void GamePanel::paintRows(IInterfaceCanvas& canvas) const {
  paintColumnHeadings(canvas);
  const std::size_t last = std::min(entryCount(), first_ + GAME_ROWS_SHOWN);
  for (std::size_t entry = first_; entry < last; ++entry) {
    const GameRowPlace place{.top = rowTopOf(entry - first_),
                             .chosen = entry == chosen_};
    if (const GameRow* game = gameAt(entry)) {
      paintGameRow(canvas, *game, place);
    } else {
      paintNewGameRow(canvas, place);
    }
  }
}

void GamePanel::paintDescription(IInterfaceCanvas& canvas) const {
  const GameRow* game = gameAt(chosen_);
  const bool enterActs = game == nullptr || isJoinable(*game);
  canvas.write(game != nullptr ? descriptionOf(*game) : NEW_GAME_DESCRIPTION,
               inPanel(GAME_PANEL_PADDING, GAME_PANEL_DESCRIPTION_TOP),
               TextStyle{.font = TextFont::Small,
                         .color = enterActs ? HUD_CYAN : PALE_BLUE});
}

std::size_t GamePanel::entryCount() const {
  return games_.games.size() + newGameRowsOf(games_);
}

const GameRow* GamePanel::gameAt(std::size_t entry) const {
  const std::size_t offset = newGameRowsOf(games_);
  if (entry < offset || entry - offset >= games_.games.size()) {
    return nullptr;
  }
  return &games_.games[entry - offset];
}

}  // namespace rtype::client
