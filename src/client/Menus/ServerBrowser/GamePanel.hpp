#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include "GameListSnapshot.hpp"
#include "GameRow.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief The panel of the games of the chosen server, over the map dimmed:
 * GAME_ROWS_SHOWN rows at a time, the chosen row highlighted, the list
 * scrolled so the chosen row stays shown, and a line that says what ENTER
 * does with it: join a waiting game, or why the others cannot be joined.
 * A new list keeps the chosen game by its number, the first one otherwise.
 */
class GamePanel {
 public:
  /** @brief Shows @p games, those of the server named @p serverName. */
  void show(const GameListSnapshot& games, std::string serverName);
  /** @brief Chooses the game @p step rows down, or up when negative, within
   * the list. */
  void move(int step);
  /** @returns The chosen game, when it can be joined. */
  [[nodiscard]] std::optional<GameRow> joinable() const;
  void paint(IInterfaceCanvas& canvas) const;

 private:
  /** @brief Paints the rows shown, from first_, the chosen one highlighted. */
  void paintRows(IInterfaceCanvas& canvas) const;
  /** @brief Paints the line under the rows, about the chosen game. */
  void paintDescription(IInterfaceCanvas& canvas) const;

  GameListSnapshot games_;
  std::string serverName_;
  std::size_t chosen_{};
  std::size_t first_{};
};

}  // namespace rtype::client
