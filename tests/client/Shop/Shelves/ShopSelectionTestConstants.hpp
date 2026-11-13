#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include "ShelfSizes.hpp"
#include "ShopShelf.hpp"
#include "ShopSpot.hpp"

namespace rtype::client::testing {

/** @brief The shelves of the shop: 22 looks, so seven rows of three and an
 * eighth holding the last look alone, and five hulls; and a shop without
 * hulls. */
constexpr ShelfSizes FULL_SHELVES{.looks = 22, .hulls = 5};
constexpr ShelfSizes SHELVES_WITHOUT_HULLS{.looks = 22, .hulls = 0};

/** @brief Looks of the locker: the first, its neighbours, the first of the
 * second row, and the last. */
constexpr ShopSpot FIRST_LOOK{.shelf = ShopShelf::Locker, .place = 0};
constexpr ShopSpot SECOND_LOOK{.shelf = ShopShelf::Locker, .place = 1};
constexpr ShopSpot THIRD_LOOK{.shelf = ShopShelf::Locker, .place = 2};
constexpr ShopSpot FOURTH_LOOK{.shelf = ShopShelf::Locker, .place = 3};
constexpr ShopSpot LAST_LOOK{.shelf = ShopShelf::Locker, .place = 21};

/** @brief Hulls of the case: the cheapest and the dearest. */
constexpr ShopSpot FIRST_HULL{.shelf = ShopShelf::HullCase, .place = 0};
constexpr ShopSpot LAST_HULL{.shelf = ShopShelf::HullCase, .place = 4};

/** @brief Presses: down to the fifth row, the first hidden one; down to the
 * seventh row; down to the eighth, the last; more than the case has hulls. */
constexpr int FOUR_ROWS = 4;
constexpr int SIX_ROWS = 6;
constexpr int SEVEN_ROWS = 7;
constexpr int MORE_THAN_THE_HULLS = 10;
constexpr std::size_t SECOND_ROW = 1;

/** @brief Pixels of the interface: the middle of the first shown cell
 * (16 + 16, 72 + 16), of the cell of the last row and second column
 * (52 + 16, 180 + 16), of the first stall (158 + 16, 206 + 18), and one
 * between the first two cells, which end at x 48 and start at x 52. */
constexpr sf::Vector2i FIRST_CELL_MIDDLE{32, 88};
constexpr sf::Vector2i LAST_ROW_SECOND_CELL_MIDDLE{68, 196};
constexpr sf::Vector2i FIRST_STALL_MIDDLE{174, 224};
constexpr sf::Vector2i BETWEEN_TWO_CELLS{50, 88};

}  // namespace rtype::client::testing
