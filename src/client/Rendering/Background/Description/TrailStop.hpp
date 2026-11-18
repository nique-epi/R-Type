#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief One color of a trail: the trail takes it up to this share of its
 * length, counted from the particle. */
struct TrailStop {
  float until{};
  sf::Color color;
};

}  // namespace rtype::client
