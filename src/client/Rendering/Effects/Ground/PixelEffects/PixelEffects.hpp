#pragma once

#include <cstddef>
#include <cstdint>
#include <random>
#include "ArrivalLines.hpp"
#include "Entity.hpp"
#include "PlacedBurst.hpp"
#include "PlacedRing.hpp"
#include "Position.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EntityRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief Creates the pixel effects: bursts of particles, shockwaves and speed
 * lines, each an entity of its own, which GroundEffectSystem moves and
 * destroys and paintGroundEffects() paints.
 *
 * An effect that waits, for the end of a freeze or of an arrival, is created
 * at once with a negative age: hidden and still until then. The draws of the
 * particles come from one generator seeded at construction, so the same seed
 * and the same calls always give the same particles. Every method adds
 * components, so none may be called during a forEach of the registry. The
 * registries must outlive it.
 */
class PixelEffects {
 public:
  /** @param seed Seed of the draws of the particles: std::random_device in
   * a game, a fixed value in a test. */
  PixelEffects(engine::EntityRegistry& entities,
               engine::ComponentRegistry& components, std::uint32_t seed);

  /**
   * @brief Bursts shape.count particles, fewer when PARTICLE_MAXIMUM would be
   * passed: each leaves in a direction drawn in the cone around `aim`, at a
   * speed drawn between shape.slowest and shape.fastest, and lives shape.life
   * give or take shape.lifeSpread of it.
   */
  void burst(const PlacedBurst& particles);

  /** @brief Opens a shockwave. */
  void wave(const PlacedRing& wave);

  /** @brief Lays one speed line on each row of SPEED_LINE_ROWS, from the left
   * edge to the ship, each living `lines.life`. */
  void speedLines(const ArrivalLines& lines);

 private:
  /** @returns How many particles are alive. */
  [[nodiscard]] std::size_t particleCount();

  /** @brief Creates one particle of a burst. */
  void addParticle(const PlacedBurst& particles);

  /** @returns A new entity at a position. */
  engine::Entity addEntityAt(game::Position position);

  engine::EntityRegistry* entities_;
  engine::ComponentRegistry* components_;
  std::mt19937 random_;
};

}  // namespace rtype::client
