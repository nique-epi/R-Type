#include "BulbChase.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <span>
#include <vector>
#include "CasinoLightConstants.hpp"
#include "ColoredPixel.hpp"

namespace rtype::client {

namespace {

/** @returns The phase of @p value, from 0 to CASINO_CHASE_PHASES - 1, a
 * negative value included. */
std::size_t phaseOf(int value) {
  const int phases = static_cast<int>(CASINO_CHASE_PHASES);
  return static_cast<std::size_t>(((value % phases) + phases) % phases);
}

}  // namespace

BulbChase::BulbChase(sf::Color unlit) : unlit_(unlit) {}

void BulbChase::add(sf::Vector2i position, int rank) {
  phases_.at(phaseOf(rank))
      .push_back(ColoredPixel{.position = position, .color = unlit_});
}

std::span<const ColoredPixel> BulbChase::unlitAt(int step) const {
  return phases_.at(phaseOf(-step));
}

std::size_t BulbChase::size() const {
  std::size_t bulbs = 0;
  for (const std::vector<ColoredPixel>& phase : phases_) {
    bulbs += phase.size();
  }
  return bulbs;
}

}  // namespace rtype::client
