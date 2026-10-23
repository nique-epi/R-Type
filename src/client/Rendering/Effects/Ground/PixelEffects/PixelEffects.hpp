#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include "Entity.hpp"
#include "GroundEffectConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

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
  /** @brief A burst of particles: its shape, where it bursts and where it
   * aims, the colors its particles go through, and how long it waits. */
  struct Particles {
    Burst shape;
    game::Position center{};
    float aim{};
    std::span<const sf::Color> colors;
    engine::Duration delay{};
  };

  /** @brief A shockwave: its shape, its center, its colors, and how long it
   * waits. */
  struct Wave {
    Ring shape;
    game::Position center{};
    std::span<const sf::Color> colors;
    engine::Duration delay{};
  };

  /** @brief The speed lines of a ship arriving: where it stands, the light
   * shade of its pilot, whether every other line is FLASH_COLOR, how long
   * they wait, and how long each then lives. */
  struct ArrivalLines {
    game::Position shipCenter{};
    sf::Color light;
    bool flashes = true;
    engine::Duration delay{};
    engine::Duration life{};
  };

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
  void burst(const Particles& particles);

  /** @brief Opens a shockwave. */
  void wave(const Wave& wave);

  /** @brief Lays one speed line on each row of SPEED_LINE_ROWS, from the left
   * edge to the ship, each living `lines.life`. */
  void speedLines(const ArrivalLines& lines);

 private:
  /** @returns How many particles are alive. */
  [[nodiscard]] std::size_t particleCount();

  /** @brief Creates one particle of a burst. */
  void addParticle(const Particles& particles);

  /** @returns A new entity at a position. */
  engine::Entity addEntityAt(game::Position position);

  engine::EntityRegistry* entities_;
  engine::ComponentRegistry* components_;
  std::mt19937 random_;
};

}  // namespace rtype::client
