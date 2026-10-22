#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include "CameraShakeConstants.hpp"
#include "EffectConstants.hpp"
#include "GroundEffectConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The shape of a burst of particles: how many, how fast they leave,
 * in logical units per second, how long they live on average and by which
 * share of that it varies, how wide the cone they leave in, in radians, and
 * whether they shrink. Where it bursts and where it aims are given then.
 */
struct Burst {
  std::size_t count{};
  float slowest{};
  float fastest{};
  engine::Duration life{};
  float lifeSpread{};
  float cone{};
  bool shrinks = false;
};

/** @brief The shape of a shockwave: its last radius, in pixels of the
 * canvas, and its life. */
struct Ring {
  int radius{};
  engine::Duration life{};
};

/**
 * @brief What a Bydo destroyed plays at its tier: how much longer its
 * explosion holds its first frame, its drops, its shockwave if any, and the
 * trauma it adds to the camera shake.
 */
struct KillTier {
  engine::Duration hold{};
  Burst drops;
  std::optional<Ring> ring;
  float trauma{};
};

/** @brief Average life of a drop or a piece of debris, give or take a third,
 * and of a spark; the cone a spark leaves in, about 57 degrees. */
constexpr engine::Duration DEBRIS_LIFE = std::chrono::milliseconds(400);
constexpr engine::Duration SPARK_LIFE = std::chrono::milliseconds(200);
constexpr float LIFE_SPREAD = 1.0F / 3.0F;
constexpr float SPARK_CONE = 1.0F;

/** @brief The bursts: drops of a Bydo at T2 and at T3, debris of a ship at
 * T4, sparks of an impact or of a hit. */
constexpr Burst KILL_DROPS{.count = 8,
                           .slowest = 120.0F,
                           .fastest = 360.0F,
                           .life = DEBRIS_LIFE,
                           .lifeSpread = LIFE_SPREAD,
                           .cone = ALL_AROUND,
                           .shrinks = true};
constexpr Burst BIG_CATCH_DROPS{.count = 16,
                                .slowest = 120.0F,
                                .fastest = 360.0F,
                                .life = DEBRIS_LIFE,
                                .lifeSpread = LIFE_SPREAD,
                                .cone = ALL_AROUND,
                                .shrinks = true};
constexpr Burst SHIP_DEBRIS{.count = 24,
                            .slowest = 180.0F,
                            .fastest = 540.0F,
                            .life = DEBRIS_LIFE,
                            .lifeSpread = LIFE_SPREAD,
                            .cone = ALL_AROUND,
                            .shrinks = true};
constexpr Burst IMPACT_SPARKS{.count = 3,
                              .slowest = 240.0F,
                              .fastest = 480.0F,
                              .life = SPARK_LIFE,
                              .lifeSpread = LIFE_SPREAD,
                              .cone = SPARK_CONE,
                              .shrinks = false};

/** @brief The shockwaves of T3 and of T4. */
constexpr Ring BIG_CATCH_RING{.radius = 24,
                              .life = std::chrono::milliseconds(300)};
constexpr Ring SHIP_DOWN_RING{.radius = 40,
                              .life = std::chrono::milliseconds(450)};

/** @brief A Bydo destroyed (T2), and every fifth kill of a chain (T3). */
constexpr KillTier KILL_TIER{.hold = KILL_HOLD,
                             .drops = KILL_DROPS,
                             .ring = std::nullopt,
                             .trauma = 0.0F};
constexpr KillTier BIG_CATCH_TIER{.hold = BIG_CATCH_HOLD,
                                  .drops = BIG_CATCH_DROPS,
                                  .ring = BIG_CATCH_RING,
                                  .trauma = BIG_CATCH_TRAUMA};

}  // namespace rtype::client
