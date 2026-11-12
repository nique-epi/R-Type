#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include "RowWindow.hpp"
#include "ShelfSizes.hpp"
#include "ShopSpot.hpp"

namespace rtype::client {

/**
 * @brief The ware the player chose in the shop, and the rows its locker
 * shows.
 *
 * The arrows move the choice among the looks of the locker, three to a row,
 * the locker scrolling to keep it in sight. Right from the right column, or
 * from the last look of a short row, and Down from the last row enter the
 * glass case on its cheapest hull; Down over a hole of the last row goes to
 * the last look. In the case, Left and Right go from hull to hull, and Up,
 * or Left from the first hull, come back to the look the choice left. The
 * pointer chooses the look of a cell shown or the hull of a stall.
 */
class ShopSelection {
 public:
  explicit ShopSelection(ShelfSizes sizes);

  [[nodiscard]] ShopSpot spot() const;
  /** @returns The rows of the locker shown. */
  [[nodiscard]] RowWindow window() const;

  /** @returns Whether the arrow @p key moved the choice; false for a key
   * that is no arrow, or an arrow against a side. */
  [[nodiscard]] bool move(sf::Keyboard::Key key);
  /** @brief Chooses the ware under @p pixel of the interface.
   * @returns Whether a ware lies there. */
  bool chooseAt(sf::Vector2i pixel);
  /** @brief Scrolls the locker @p step rows down, up for a negative step,
   * and keeps a look chosen among the rows shown. */
  void scroll(int step);

 private:
  /** @brief Moves the choice inside the glass case, or back to the locker. */
  void moveInCase(sf::Keyboard::Key key);

  ShelfSizes sizes_;
  ShopSpot spot_;
  std::size_t lookLeft_{};
  RowWindow rows_;
};

}  // namespace rtype::client
