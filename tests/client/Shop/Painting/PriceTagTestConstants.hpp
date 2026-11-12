#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>

namespace rtype::client::testing {

/** @brief A price of four digits and the dearest hull, with the widths of
 * their tags: 4 before the digits, 4 per digit, 3 after them. */
constexpr std::uint32_t FOUR_DIGIT_PRICE = 1500;
constexpr int FOUR_DIGIT_TAG_WIDTH = 23;
constexpr std::uint32_t DEAREST_HULL_PRICE = 10000;
constexpr int DEAREST_HULL_TAG_WIDTH = 27;

/** @brief Where a tag of 1500 is painted in the test, and the pixels it may
 * cover: 23 x 7 ending at that corner. */
constexpr sf::Vector2i TAG_CORNER{100, 50};
constexpr sf::IntRect TAG_AREA{{77, 50}, {23, 7}};

}  // namespace rtype::client::testing
