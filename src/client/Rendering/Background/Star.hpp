#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>

namespace rtype::client {

/**
 * @brief Depth of a star, from the farthest to the nearest: the farther, the
 * slower and the darker.
 */
enum class StarDepth : std::uint8_t { Far, Middle, Near };

/** @brief One star of the background. */
struct Star {
  /** Position in background pixels; x decreases as the sky scrolls. */
  sf::Vector2f position;
  StarDepth depth{};
  /** Step of the twinkle cycle at which the star twinkles. */
  std::int64_t twinklePhase{};
  /** A warm near star is drawn gold instead of white. */
  bool warm{};
};

/** @brief Depth, number and speed of the stars of one layer. */
struct StarLayer {
  StarDepth depth{};
  std::size_t count{};
  /** Speed towards the left, in background pixels per second. */
  float speed{};
};

}  // namespace rtype::client
