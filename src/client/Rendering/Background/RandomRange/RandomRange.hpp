#pragma once

#include <SFML/System/Vector2.hpp>
#include <random>

namespace rtype::client {

/** @returns A value drawn uniformly between minimum and maximum. */
[[nodiscard]] float randomBetween(std::mt19937& random, float minimum,
                                  float maximum);

/**
 * @returns A point drawn uniformly in the rectangle between two corners. Its
 * column is drawn before its row, whatever order a compiler evaluates
 * function arguments in.
 */
[[nodiscard]] sf::Vector2f randomPointBetween(std::mt19937& random,
                                              sf::Vector2f minimum,
                                              sf::Vector2f maximum);

/** @returns true with the given probability, from 0 to 1. */
[[nodiscard]] bool randomChance(std::mt19937& random, float probability);

}  // namespace rtype::client
