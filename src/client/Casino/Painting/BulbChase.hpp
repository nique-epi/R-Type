#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <span>
#include <vector>
#include "CasinoLightConstants.hpp"
#include "ColoredPixel.hpp"

namespace rtype::client {

/**
 * @brief Bulbs that chase: each bulb has a rank, its place along the chase,
 * and at a step of the chase the bulbs whose rank plus the step is a whole
 * number of CASINO_CHASE_PHASES are out; the next step puts out the bulbs one
 * rank lower, so the pattern runs toward the lower ranks.
 *
 * The bulbs are kept already in the color of a bulb out, sorted by phase:
 * a step paints one list as it is, with no pixel built per frame.
 */
class BulbChase {
 public:
  /** @param unlit The color a bulb takes while it is out. */
  explicit BulbChase(sf::Color unlit);

  /** @brief Adds the bulb lit at @p position, at @p rank along the chase. */
  void add(sf::Vector2i position, int rank);
  /** @returns The bulbs out at @p step, in the color of a bulb out. */
  [[nodiscard]] std::span<const ColoredPixel> unlitAt(int step) const;
  /** @returns How many bulbs the chase holds. */
  [[nodiscard]] std::size_t size() const;

 private:
  sf::Color unlit_;
  std::array<std::vector<ColoredPixel>, CASINO_CHASE_PHASES> phases_;
};

}  // namespace rtype::client
