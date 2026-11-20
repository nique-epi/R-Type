#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <chrono>

namespace rtype::client::testing {

/**
 * @brief Times of the master in the cycle of greetings of the croupier, a
 * cycle of 2.6 s whose last 0.6 s is his greeting, four frames of 150 ms:
 * 250 ms into a cycle, his second idle frame; 2150 ms into it, the second
 * frame of his greeting; 2150 ms into it again, on a wheel that spins, his
 * idle frame of that time, the first, since 10 408 idle frames of 250 ms
 * passed.
 */
constexpr std::chrono::milliseconds INTO_THE_IDLE_OF_A_CYCLE{2'600'250};
constexpr std::chrono::milliseconds INTO_THE_GREETING_OF_A_CYCLE{2'602'150};

/** @brief The frames of the croupier, the top 76 rows of a frame of 80 x 112
 * of the sheet of the butler: the second idle frame, the second frame of his
 * greeting, the first idle frame, and the third frame of his greeting he
 * holds with less motion while the bets are open. */
constexpr sf::IntRect SECOND_IDLE_FRAME({80, 0}, {80, 76});
constexpr sf::IntRect SECOND_GREETING_FRAME({80, 112}, {80, 76});
constexpr sf::IntRect FIRST_IDLE_FRAME({0, 0}, {80, 76});
constexpr sf::IntRect HELD_GREETING_FRAME({160, 112}, {80, 76});

}  // namespace rtype::client::testing
