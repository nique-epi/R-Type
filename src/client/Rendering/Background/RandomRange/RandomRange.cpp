#include "RandomRange.hpp"
#include <SFML/System/Vector2.hpp>
#include <random>

namespace rtype::client {

float randomBetween(std::mt19937& random, float minimum, float maximum) {
  std::uniform_real_distribution<float> distribution(minimum, maximum);
  return distribution(random);
}

sf::Vector2f randomPointBetween(std::mt19937& random, sf::Vector2f minimum,
                                sf::Vector2f maximum) {
  const float column = randomBetween(random, minimum.x, maximum.x);
  const float row = randomBetween(random, minimum.y, maximum.y);
  return {column, row};
}

bool randomChance(std::mt19937& random, float probability) {
  std::bernoulli_distribution distribution(probability);
  return distribution(random);
}

}  // namespace rtype::client
