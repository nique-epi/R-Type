#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "ClothPlace.hpp"

namespace rtype::client {

/** @returns The pixel of the interface the chips of a bet on @p place are
 * piled on: the middle of a number, of a column, of a dozen or of an even
 * chance, or the point of a line or of a corner. */
[[nodiscard]] sf::Vector2i spotOf(ClothPlace place);

/** @returns What the cursor frames on @p place: the box of a number, of the
 * zero, of a column, of a dozen or of an even chance, its lines included, or
 * a box of CLOTH_LINE_SPOT_SIZE around the point of a line or of a corner. */
[[nodiscard]] sf::IntRect areaOf(ClothPlace place);

/** @returns The box of @p number on the cloth, 0 to HIGHEST_ROULETTE_NUMBER,
 * its lines included. */
[[nodiscard]] sf::IntRect numberCellOf(std::uint8_t number);

}  // namespace rtype::client
