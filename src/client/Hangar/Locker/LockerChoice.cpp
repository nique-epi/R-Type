#include "LockerChoice.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include "LockerConstants.hpp"
#include "RowWindow.hpp"
#include "ShipLookConstants.hpp"

namespace rtype::client {

using identifiers::SHIP_LOOK_ITEM_COUNT;

namespace {

/** @returns @p wanted when it holds an item, else @p current. */
std::size_t itemOrStay(std::size_t current, std::size_t wanted) {
  return wanted < SHIP_LOOK_ITEM_COUNT ? wanted : current;
}

}  // namespace

sf::IntRect lockerCellArea(std::size_t slot) {
  const auto column = static_cast<int>(slot % LOCKER_COLUMN_COUNT);
  const auto row = static_cast<int>(slot / LOCKER_COLUMN_COUNT);
  const int pitch = LOCKER_CELL_SIZE + LOCKER_GAP;
  return {LOCKER_TOP_LEFT + sf::Vector2i(LOCKER_PADDING + (column * pitch),
                                         LOCKER_PLATE_HEIGHT + (row * pitch)),
          {LOCKER_CELL_SIZE, LOCKER_CELL_SIZE}};
}

std::optional<std::size_t> lockerItemAt(sf::Vector2i pixel,
                                        std::size_t firstRow) {
  for (std::size_t slot = 0; slot < LOCKER_SLOT_COUNT; ++slot) {
    const std::size_t item = (firstRow * LOCKER_COLUMN_COUNT) + slot;
    if (item < SHIP_LOOK_ITEM_COUNT && lockerCellArea(slot).contains(pixel)) {
      return item;
    }
  }
  return std::nullopt;
}

std::size_t lockerRowOf(std::size_t item) { return item / LOCKER_COLUMN_COUNT; }

std::size_t lockerItemShownIn(std::size_t item, RowWindow rows) {
  if (showsRow(rows, lockerRowOf(item))) {
    return item;
  }
  const std::size_t row =
      lockerRowOf(item) < rows.first ? rows.first : rows.first + rows.shown - 1;
  return std::min((row * LOCKER_COLUMN_COUNT) + (item % LOCKER_COLUMN_COUNT),
                  SHIP_LOOK_ITEM_COUNT - 1);
}

std::optional<std::size_t> lockerItemTowards(std::size_t from,
                                             sf::Keyboard::Key key) {
  const std::size_t column = from % LOCKER_COLUMN_COUNT;
  switch (key) {
    case sf::Keyboard::Key::Left:
      return column > 0 ? from - 1 : from;
    case sf::Keyboard::Key::Right:
      return column + 1 < LOCKER_COLUMN_COUNT ? itemOrStay(from, from + 1)
                                              : from;
    case sf::Keyboard::Key::Up:
      return from >= LOCKER_COLUMN_COUNT ? from - LOCKER_COLUMN_COUNT : from;
    case sf::Keyboard::Key::Down:
      return itemOrStay(from, from + LOCKER_COLUMN_COUNT);
    default:
      return std::nullopt;
  }
}

}  // namespace rtype::client
