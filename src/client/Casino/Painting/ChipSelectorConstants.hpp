#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <cstddef>
#include <cstdint>

namespace rtype::client {

/**
 * @brief The chips of a selector, the cashier's and the table's, from the
 * left: their values, the key that chooses each and the name on its keycap.
 */
constexpr std::size_t CHIP_VALUE_COUNT = 5;
constexpr std::array<std::uint32_t, CHIP_VALUE_COUNT> CHIP_VALUES{1, 5, 25, 100,
                                                                  500};
constexpr std::array CHIP_KEYS{sf::Keyboard::Key::Num1, sf::Keyboard::Key::Num2,
                               sf::Keyboard::Key::Num3, sf::Keyboard::Key::Num4,
                               sf::Keyboard::Key::Num5};
constexpr std::array<const char*, CHIP_VALUE_COUNT> CHIP_KEY_NAMES{
    "1", "2", "3", "4", "5"};
static_assert(CHIP_KEYS.size() == CHIP_VALUE_COUNT,
              "every chip of the selector has its key");

/**
 * @brief How the chips of a selector lie: CHIP_SPACING apart from the left of
 * one to the left of the next, the chosen one CHOSEN_CHIP_LIFT higher and in
 * corners CHOSEN_CHIP_CORNERS_MARGIN around it, each with the keycap of its
 * key CHIP_KEY_CAP_DROP under the top of the row, CHIP_KEY_CAP_INSET in.
 */
constexpr int CHIP_SPACING = 30;
constexpr int CHOSEN_CHIP_LIFT = 2;
constexpr int CHOSEN_CHIP_CORNERS_MARGIN = 3;
constexpr int CHIP_KEY_CAP_DROP = 24;
constexpr int CHIP_KEY_CAP_INSET = 6;

}  // namespace rtype::client
