#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include "GameListSnapshot.hpp"
#include "GamePanelLayout.hpp"
#include "GameRow.hpp"
#include "IInterfaceCanvas.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/**
 * @brief The panel of the games of the chosen server, over the map dimmed:
 * once the master answered, + NEW GAME first, then the games; GAME_ROWS_SHOWN
 * rows at a time, the chosen row highlighted, the list scrolled so the chosen
 * row stays shown, and a line that says what ENTER does with it: set the rules
 * of a new game, join a waiting game, named with its rules, or why the others
 * cannot be joined. The pointer chooses the row it lies over. It opens on + NEW
 * GAME; a new list keeps the chosen game by its number, + NEW GAME otherwise.
 */
class GamePanel {
 public:
  /** @param metrics The text size the panel is laid out at, which lays a game
   * on two lines and shows fewer of them above the size the fonts are drawn
   * at. */
  explicit GamePanel(TextMetrics metrics = {});

  /** @brief Shows @p games, those of the server named @p serverName. */
  void show(const GameListSnapshot& games, std::string serverName);
  /** @brief Chooses the row @p step rows down, or up when negative, within
   * the list. */
  void move(int step);
  /** @returns The chosen game, when it can be joined. */
  [[nodiscard]] std::optional<GameRow> joinable() const;
  /** @brief Chooses the row shown under @p pixel, a pixel of the interface.
   * @returns Whether a row lies there; nothing changes when none does. */
  bool chooseAt(sf::Vector2i pixel);
  /** @returns Whether the chosen row is + NEW GAME. */
  [[nodiscard]] bool newGameChosen() const;
  void paint(IInterfaceCanvas& canvas) const;

 private:
  /** @brief Paints the rows shown, from first_, the chosen one highlighted. */
  void paintRows(IInterfaceCanvas& canvas) const;
  /** @brief Paints the line under the rows, about the chosen game. */
  void paintDescription(IInterfaceCanvas& canvas) const;
  /** @returns The rows the panel holds: + NEW GAME once the master answered,
   * then the games. */
  [[nodiscard]] std::size_t entryCount() const;
  /** @returns The game on row @p entry, which lives as long as the list
   * shown; nothing for + NEW GAME or past the last row. */
  [[nodiscard]] const GameRow* gameAt(std::size_t entry) const;

  GamePanelLayout layout_;
  GameListSnapshot games_;
  std::string serverName_;
  std::size_t chosen_{};
  std::size_t first_{};
};

}  // namespace rtype::client
