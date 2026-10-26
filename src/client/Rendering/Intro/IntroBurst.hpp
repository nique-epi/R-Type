#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>
#include "IntroBlast.hpp"
#include "IntroSpark.hpp"

namespace rtype::client {

/** @brief An explosion of the intro, from one place at one moment of the
 * scene: its explosions of the sheet, its sparks of fire, and the purple
 * drops of the Bydos. */
struct IntroBurst {
  sf::Vector2f center;
  float start{};
  std::vector<IntroBlast> blasts;
  std::vector<IntroSpark> sparks;
  std::vector<IntroSpark> drops;
};

}  // namespace rtype::client
