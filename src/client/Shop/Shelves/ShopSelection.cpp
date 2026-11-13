#include "ShopSelection.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include "RowWindow.hpp"
#include "ShelfSizes.hpp"
#include "ShopLayout.hpp"
#include "ShopLayoutConstants.hpp"
#include "ShopShelf.hpp"
#include "ShopSpot.hpp"

namespace rtype::client {

namespace {

/** @returns The cheapest hull of the glass case, where the choice enters
 * it. */
ShopSpot firstHull() { return {.shelf = ShopShelf::HullCase, .place = 0}; }

/** @returns The rows the looks fill, the last one short when they do not
 * fill it. */
std::size_t lookRowCount(std::size_t looks) {
  return (looks + SHOP_LOCKER_COLUMN_COUNT - 1) / SHOP_LOCKER_COLUMN_COUNT;
}

ShopSpot lookAt(std::size_t place) {
  return {.shelf = ShopShelf::Locker, .place = place};
}

/** @returns Where @p key moves the choice from the look at @p place: a look,
 * the first hull when the case is entered, or @p place itself. */
ShopSpot fromLook(std::size_t place, sf::Keyboard::Key key, ShelfSizes sizes) {
  const std::size_t column = place % SHOP_LOCKER_COLUMN_COUNT;
  const ShopSpot orStay = sizes.hulls > 0 ? firstHull() : lookAt(place);
  switch (key) {
    case sf::Keyboard::Key::Left:
      return lookAt(column > 0 ? place - 1 : place);
    case sf::Keyboard::Key::Right:
      return column + 1 < SHOP_LOCKER_COLUMN_COUNT && place + 1 < sizes.looks
                 ? lookAt(place + 1)
                 : orStay;
    case sf::Keyboard::Key::Up:
      return lookAt(place >= SHOP_LOCKER_COLUMN_COUNT
                        ? place - SHOP_LOCKER_COLUMN_COUNT
                        : place);
    case sf::Keyboard::Key::Down:
      if (place + SHOP_LOCKER_COLUMN_COUNT < sizes.looks) {
        return lookAt(place + SHOP_LOCKER_COLUMN_COUNT);
      }
      return (place / SHOP_LOCKER_COLUMN_COUNT) + 1 < lookRowCount(sizes.looks)
                 ? lookAt(sizes.looks - 1)
                 : orStay;
    default:
      return lookAt(place);
  }
}

}  // namespace

ShopSelection::ShopSelection(ShelfSizes sizes)
    : sizes_(sizes),
      rows_{.first = 0,
            .shown = SHOP_LOCKER_SHOWN_ROW_COUNT,
            .count = lookRowCount(sizes.looks)} {}

ShopSpot ShopSelection::spot() const { return spot_; }

RowWindow ShopSelection::window() const { return rows_; }

bool ShopSelection::move(sf::Keyboard::Key key) {
  const ShopSpot before = spot_;
  if (spot_.shelf == ShopShelf::HullCase) {
    moveInCase(key);
  } else if (sizes_.looks > 0) {
    spot_ = fromLook(spot_.place, key, sizes_);
    if (spot_.shelf == ShopShelf::HullCase) {
      lookLeft_ = before.place;
    }
  }
  if (spot_.shelf == ShopShelf::Locker) {
    rows_ = rowsShowing(rows_, spot_.place / SHOP_LOCKER_COLUMN_COUNT);
  }
  return spot_ != before;
}

bool ShopSelection::chooseAt(sf::Vector2i pixel) {
  if (const std::optional<std::size_t> hull =
          shopStallAt(pixel, sizes_.hulls)) {
    if (spot_.shelf == ShopShelf::Locker) {
      lookLeft_ = spot_.place;
    }
    spot_ = {.shelf = ShopShelf::HullCase, .place = *hull};
    return true;
  }
  const std::optional<std::size_t> slot = shopCellAt(pixel);
  if (!slot.has_value()) {
    return false;
  }
  const std::size_t look = (rows_.first * SHOP_LOCKER_COLUMN_COUNT) + *slot;
  if (look >= sizes_.looks) {
    return false;
  }
  spot_ = lookAt(look);
  return true;
}

void ShopSelection::scroll(int step) {
  rows_ = rowsScrolledBy(rows_, step);
  if (spot_.shelf != ShopShelf::Locker || sizes_.looks == 0) {
    return;
  }
  const std::size_t row =
      std::clamp(spot_.place / SHOP_LOCKER_COLUMN_COUNT, rows_.first,
                 rows_.first + rows_.shown - 1);
  spot_.place = std::min((row * SHOP_LOCKER_COLUMN_COUNT) +
                             (spot_.place % SHOP_LOCKER_COLUMN_COUNT),
                         sizes_.looks - 1);
}

void ShopSelection::moveInCase(sf::Keyboard::Key key) {
  switch (key) {
    case sf::Keyboard::Key::Left:
      spot_ = spot_.place > 0 ? ShopSpot{.shelf = ShopShelf::HullCase,
                                         .place = spot_.place - 1}
                              : lookAt(lookLeft_);
      break;
    case sf::Keyboard::Key::Right:
      if (spot_.place + 1 < sizes_.hulls) {
        ++spot_.place;
      }
      break;
    case sf::Keyboard::Key::Up:
      spot_ = lookAt(lookLeft_);
      break;
    default:
      break;
  }
}

}  // namespace rtype::client
