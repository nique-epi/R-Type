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
#include "GamePanelLayout.hpp"
#include "GameRow.hpp"
#include "GameRulesLine.hpp"
#include "GameRulesLineConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InkTag.hpp"
#include "InterfaceColorConstants.hpp"
#include "ListedGameState.hpp"
#include "SectorMapConstants.hpp"
#include "ServerListPhase.hpp"
#include "ServerMapFrames.hpp"
#include "ServerMapScreenConstants.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "WrappedLines.hpp"

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

sf::Vector2i inPanel(const GamePanelLayout& layout, int across, int down) {
  return layout.area.position + sf::Vector2i{across, down};
}

/** @returns Where the text of the row @p shown rows under the first one shown
 * stands, down from the top of the panel. */
int rowTopOf(const GamePanelLayout& layout, std::size_t shown) {
  return layout.firstRow + (static_cast<int>(shown) * layout.rowPitch);
}

/** @returns The band of the row @p shown rows under the first one shown, which
 * the pointer chooses it in: as wide as its highlight, and as tall as the
 * pitch of the rows, so no click falls between two rows. */
sf::IntRect rowBandOf(const GamePanelLayout& layout, std::size_t shown) {
  return {
      inPanel(layout, GAME_ROW_HIGHLIGHT_LEFT,
              rowTopOf(layout, shown) - GAME_ROW_HIGHLIGHT_RISE),
      {layout.area.size.x - (2 * GAME_ROW_HIGHLIGHT_LEFT), layout.rowPitch}};
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

/** @brief Writes the headings of the columns, as drawn; none when a game
 * takes two lines, which name nothing in columns. */
void paintColumnHeadings(IInterfaceCanvas& canvas,
                         const GamePanelLayout& layout) {
  if (layout.stacked) {
    return;
  }
  const TextStyle heading{.font = TextFont::Small, .color = BLUE};
  canvas.write(GAME_COLUMN_LABEL,
               inPanel(layout, GAME_NAME_COLUMN, GAME_PANEL_COLUMNS_TOP),
               heading);
  canvas.write(PILOTS_COLUMN_LABEL,
               inPanel(layout, GAME_PILOTS_COLUMN, GAME_PANEL_COLUMNS_TOP),
               heading);
  canvas.write(STATE_COLUMN_LABEL,
               inPanel(layout, GAME_STATE_COLUMN, GAME_PANEL_COLUMNS_TOP),
               heading);
}

/** @brief Where a row of the panel lies, and whether it is the chosen one. */
struct GameRowPlace {
  GamePanelLayout layout;
  int top{};
  bool chosen{};
};

/** @brief Paints the highlight of the chosen row of @p place. */
void paintRowHighlight(IInterfaceCanvas& canvas, const GameRowPlace& place) {
  const GamePanelLayout& layout = place.layout;
  canvas.fill({inPanel(layout, GAME_ROW_HIGHLIGHT_LEFT,
                       place.top - GAME_ROW_HIGHLIGHT_RISE),
               {layout.area.size.x - (2 * GAME_ROW_HIGHLIGHT_LEFT),
                layout.highlightHeight}},
              EDGE_BLUE);
}

/** @brief Paints the pilots of @p game of its seats, and its state as a word
 * on ink: in their columns, or after each other on the second line of the row
 * when a game takes two lines. */
void paintGameSeats(IInterfaceCanvas& canvas, const GameRow& game,
                    const GameRowPlace& place) {
  const GamePanelLayout& layout = place.layout;
  const std::string pilots = std::to_string(game.playerCount) +
                             PILOTS_SEPARATOR +
                             std::to_string(game.maximumPlayers);
  const sf::Vector2i pilotsAt =
      layout.stacked
          ? inPanel(layout, GAME_NAME_COLUMN, place.top + layout.secondLine)
          : inPanel(layout, GAME_PILOTS_COLUMN, place.top);
  canvas.write(pilots, pilotsAt,
               TextStyle{.font = TextFont::Regular,
                         .color = place.chosen ? INK : STAR_WHITE});
  const sf::Vector2i stateAt =
      layout.stacked
          ? pilotsAt +
                sf::Vector2i{canvas.textWidth(pilots, TextFont::Regular) +
                                 GAME_STATE_GAP,
                             0}
          : inPanel(layout, GAME_STATE_COLUMN, place.top);
  paintInkTag(canvas,
              InkTag{.word = stateWordOf(game.state),
                     .topLeft = stateAt,
                     .wordColor = isJoinable(game) ? HUD_CYAN : STAR_GOLD});
}

/** @brief Paints @p game on its row: its name, its pilots of its seats, and
 * its state as a word on ink; the chosen row on a highlight, in ink. */
void paintGameRow(IInterfaceCanvas& canvas, const GameRow& game,
                  const GameRowPlace& place) {
  if (place.chosen) {
    paintRowHighlight(canvas, place);
  }
  canvas.write(game.name, inPanel(place.layout, GAME_NAME_COLUMN, place.top),
               TextStyle{.font = TextFont::Regular,
                         .color = place.chosen ? INK : PALE_BLUE});
  paintGameSeats(canvas, game, place);
}

/** @brief Paints + NEW GAME on its row, in cyan, or in ink on the highlight
 * when it is chosen. */
void paintNewGameRow(IInterfaceCanvas& canvas, const GameRowPlace& place) {
  if (place.chosen) {
    paintRowHighlight(canvas, place);
  }
  canvas.write(NEW_GAME_ROW_LABEL,
               inPanel(place.layout, GAME_NAME_COLUMN, place.top),
               TextStyle{.font = TextFont::Regular,
                         .color = place.chosen ? INK : HUD_CYAN});
}

}  // namespace

GamePanel::GamePanel(TextMetrics metrics)
    : layout_(gamePanelLayoutOf(metrics)) {}

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
  } else if (chosen_ >= first_ + layout_.rowsShown) {
    first_ = chosen_ + 1 - layout_.rowsShown;
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
  const std::size_t last = std::min(entryCount(), first_ + layout_.rowsShown);
  for (std::size_t entry = first_; entry < last; ++entry) {
    if (rowBandOf(layout_, entry - first_).contains(pixel)) {
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
  CutCornerBox(layout_.area, {.border = HUD_CYAN, .inside = SKY_COLOR})
      .paint(canvas);
  canvas.write(serverName_,
               inPanel(layout_, GAME_PANEL_PADDING, GAME_PANEL_TITLE_TOP),
               TextStyle{.font = TextFont::Regular, .color = HUD_CYAN});
  canvas.fill({inPanel(layout_, GAME_PANEL_PADDING, layout_.ruleTop),
               {layout_.area.size.x - (2 * GAME_PANEL_PADDING), 1}},
              DEEP_BLUE);
  const bool offersNewGame = newGameRowsOf(games_) > 0;
  if (offersNewGame) {
    paintRows(canvas);
    paintDescription(canvas);
  }
  if (const std::optional<const char*> message = messageOf(games_.phase)) {
    canvas.write(*message,
                 inPanel(layout_, layout_.area.size.x / 2,
                         offersNewGame ? rowTopOf(layout_, EMPTY_MESSAGE_ROWS)
                                       : layout_.firstRow),
                 TextStyle{.font = TextFont::Regular,
                           .color = PALE_BLUE,
                           .alignment = TextAlignment::Center});
  }
}

void GamePanel::paintRows(IInterfaceCanvas& canvas) const {
  paintColumnHeadings(canvas, layout_);
  const std::size_t last = std::min(entryCount(), first_ + layout_.rowsShown);
  for (std::size_t entry = first_; entry < last; ++entry) {
    const GameRowPlace place{.layout = layout_,
                             .top = rowTopOf(layout_, entry - first_),
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
  const std::string description =
      game != nullptr ? descriptionOf(*game) : NEW_GAME_DESCRIPTION;
  const TextStyle style{.font = TextFont::Small,
                        .color = enterActs ? HUD_CYAN : PALE_BLUE};
  sf::Vector2i anchor =
      inPanel(layout_, GAME_PANEL_PADDING, layout_.descriptionTop);
  if (!layout_.descriptionWidth.has_value()) {
    canvas.write(description, anchor, style);
    return;
  }
  for (const std::string& line : wrappedLinesOf(
           description,
           {.width = *layout_.descriptionWidth, .font = TextFont::Small},
           canvas)) {
    canvas.write(line, anchor, style);
    anchor.y += lineHeightOf(canvas.textMetrics()) + SERVER_MAP_LINE_GAP;
  }
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
