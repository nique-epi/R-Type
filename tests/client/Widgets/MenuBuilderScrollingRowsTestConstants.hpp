#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>

namespace rtype::client::testing {

/** @brief The labels of a panel of nine rows, one more than its eight. */
constexpr std::array<const char*, 9> NINE_ROW_LABELS{"ROW 1", "ROW 2", "ROW 3",
                                                     "ROW 4", "ROW 5", "ROW 6",
                                                     "ROW 7", "ROW 8", "ROW 9"};

/** @brief Where the label of the first and of the eighth shown row is
 * written: the tabs at 24 + 8 + 8 + 1 + 9 = 50, the panel 16 lower at 66,
 * the first row 4 inside it at 70, its label 4 lower and 8 right of the
 * row's left at 28; each row 18 below the one above, the eighth at 196. */
constexpr sf::Vector2i FIRST_SHOWN_ROW_LABEL(36, 74);
constexpr sf::Vector2i LAST_SHOWN_ROW_LABEL(36, 200);

/** @brief Down presses that move the selection from the first row to the
 * ninth, and the order of the second row among the buttons. */
constexpr std::size_t PRESSES_TO_THE_NINTH_ROW = 8;
constexpr std::size_t SECOND_ROW = 1;

/** @brief One notch of the wheel toward the player, which scrolls down. */
constexpr float ONE_NOTCH_DOWN = -1.0F;

/** @brief A pixel inside the first shown row, left of its control. */
constexpr sf::Vector2i INSIDE_THE_FIRST_SHOWN_ROW(60, 80);

}  // namespace rtype::client::testing
