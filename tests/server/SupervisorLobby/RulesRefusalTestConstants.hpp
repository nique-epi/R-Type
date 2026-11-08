#pragma once

#include <cstdint>

/**
 * @brief Lives at the bounds of what a game accepts, 1 to 9: no life, the
 * fewest and the most; LIVES_PAST_THE_MOST is one past the most.
 */
constexpr std::uint8_t NO_LIFE = 0;
constexpr std::uint8_t ONE_LIFE = 1;
constexpr std::uint8_t NINE_LIVES = 9;
