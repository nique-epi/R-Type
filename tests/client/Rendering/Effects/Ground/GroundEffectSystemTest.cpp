#include <gtest/gtest.h>
#include <chrono>
#include "ComponentRegistry.hpp"
#include "EffectTestConstants.hpp"
#include "Entity.hpp"
#include "GroundEffectScene.hpp"
#include "GroundEffectSystemTestConstants.hpp"
#include "Particle.hpp"
#include "Position.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::Particle;
using rtype::engine::Duration;
using rtype::engine::Entity;
using rtype::game::Position;

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
