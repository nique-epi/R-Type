#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <array>
#include <chrono>
#include "ComponentRegistry.hpp"
#include "EffectLengths.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "GroundEffectSystem.hpp"
#include "Particle.hpp"
#include "Position.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::GroundEffectSystem;
using rtype::client::Particle;
using rtype::engine::ComponentRegistry;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::Position;

namespace {

constexpr Position START{.x = 400.0F, .y = 200.0F};
constexpr float SPEED = 600.0F;
constexpr Duration SIXTIETH = std::chrono::nanoseconds(16'666'667);
constexpr Duration THIRTIETH = std::chrono::nanoseconds(33'333'333);
constexpr std::array RAMP{sf::Color::White};
constexpr float SPEED_KEPT_PER_SIXTIETH = 0.9F;
constexpr float SPEED_TOLERANCE = 1e-2F;

class GroundEffectScene {
 public:
  Entity addParticle(Duration age) {
    const Entity entity = entities_.create();
    components_.add(entity, START);
    components_.add(entity, Particle{.velocity = {SPEED, 0.0F},
                                     .age = age,
                                     .life = A_PARTICLE_LIFE,
                                     .colors = RAMP,
                                     .shrinks = false});
    return entity;
  }

  void run(Duration elapsed) { system_.update(components_, elapsed); }

  ComponentRegistry& components() { return components_; }

  [[nodiscard]] bool isAlive(Entity entity) const {
    return entities_.isAlive(entity);
  }

 private:
  EntityRegistry entities_;
  ComponentRegistry components_{entities_};
  GroundEffectSystem system_;
};

}  // namespace

/**
 * Given a particle waiting 100 ms
 * When 16 ms pass
 * Then it has not moved
 */
TEST(GroundEffectSystem, AWaitingParticleStaysStill) {
  GroundEffectScene scene;
  const Entity particle = scene.addParticle(-std::chrono::milliseconds(100));

  scene.run(ONE_FRAME);

  EXPECT_EQ(scene.components().get<Position>(particle)->x, START.x);
}

/**
 * Given a particle leaving at 600 units a second
 * When a sixtieth of a second passes
 * Then it moved 10 units and kept 90 % of its speed
 */
TEST(GroundEffectSystem, AParticleMovesThenSlowsDown) {
  GroundEffectScene scene;
  const Entity particle = scene.addParticle(Duration::zero());

  scene.run(SIXTIETH);

  EXPECT_NEAR(scene.components().get<Position>(particle)->x,
              START.x + (SPEED / 60.0F), SPEED_TOLERANCE);
  EXPECT_NEAR(scene.components().get<Particle>(particle)->velocity.x,
              SPEED * SPEED_KEPT_PER_SIXTIETH, SPEED_TOLERANCE);
}

/**
 * Given two particles leaving at the same speed
 * When one steps two sixtieths of a second and the other one thirtieth
 * Then both kept the same speed: friction does not depend on the frame rate
 */
TEST(GroundEffectSystem, FrictionDoesNotDependOnTheFrameRate) {
  GroundEffectScene scene;
  const Entity stepped = scene.addParticle(Duration::zero());
  GroundEffectScene other;
  const Entity jumped = other.addParticle(Duration::zero());

  scene.run(SIXTIETH);
  scene.run(SIXTIETH);
  other.run(THIRTIETH);

  EXPECT_NEAR(scene.components().get<Particle>(stepped)->velocity.x,
              other.components().get<Particle>(jumped)->velocity.x,
              SPEED_TOLERANCE);
}

/**
 * Given a particle living 400 ms
 * When 400 ms pass
 * Then it is destroyed
 */
TEST(GroundEffectSystem, AParticleIsDestroyedAtTheEndOfItsLife) {
  GroundEffectScene scene;
  const Entity particle = scene.addParticle(Duration::zero());

  scene.run(A_PARTICLE_LIFE);

  EXPECT_FALSE(scene.isAlive(particle));
}
