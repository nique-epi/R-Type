#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <random>
#include <vector>
#include "ILayerCanvas.hpp"
#include "ILayerContent.hpp"
#include "IParticleEmitter.hpp"
#include "LayerFrame.hpp"
#include "Particle.hpp"
#include "ParticleContent.hpp"
#include "ParticleLayerParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief A layer of particles, as its description says (see
 * ParticleContent).
 *
 * A population holds its particles from the start. They stay where they are
 * in their layer, so they move with it; in a layer that repeats, each one
 * that leaves on the left comes back from the right at a new height, half of
 * what the repeat is wider than the background out of sight on each side, so
 * the sky never shows the same stars twice. The particles of a group are
 * born one at a time, when emit() is asked, fly on their own and end (see
 * hasEnded()).
 *
 * Nothing depends on the frame rate: everything moves by the time given to
 * advance().
 */
class ParticleLayer final : public ILayerContent, public IParticleEmitter {
 public:
  explicit ParticleLayer(ParticleLayerParts parts);

  void advance(engine::Duration elapsed, const LayerFrame& frame) override;
  void paint(ILayerCanvas& canvas, const LayerFrame& frame) const override;
  [[nodiscard]] int leftEdge() const override;

  void emit() override;
  [[nodiscard]] std::size_t aliveCount() const override;

  /** @returns The particles, in the order they were born. */
  [[nodiscard]] const std::vector<Particle>& particles() const;

  /** @returns Where @p particle lies on the background when the origin of
   * the layer lies on @p column: for one of a population, on the copy of the
   * layer that shows. */
  [[nodiscard]] sf::Vector2f placeOf(const Particle& particle,
                                     int column) const;

 private:
  /** @returns How many times a particle of a population on the column
   * @p home of its layer came back when the origin of the layer lies on
   * @p column. */
  [[nodiscard]] int lapOf(float home, int column) const;

  /** @brief Gives a new height to each particle of the population that came
   * back since the last time. */
  void bringBack(int column);

  ParticleContent description_;
  int repeat_;
  std::mt19937 random_;
  std::vector<Particle> particles_;
  engine::Duration age_{};
};

}  // namespace rtype::client
