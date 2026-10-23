#pragma once

#include <chrono>
#include <optional>
#include "Burst.hpp"
#include "CameraShakeConstants.hpp"
#include "EffectConstants.hpp"
#include "GroundEffectConstants.hpp"
#include "KillTier.hpp"
#include "Ring.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

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
