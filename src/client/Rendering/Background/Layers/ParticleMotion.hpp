#pragma once

#include <random>
#include "Particle.hpp"
#include "ParticleContent.hpp"

namespace rtype::client {

/** @returns A particle of the population @p description describes, on a
 * whole column and at a height of its area, both drawn from @p random, with
 * its twinkle phase and its sprite. */
[[nodiscard]] Particle settledParticle(const ParticleContent& description,
                                       std::mt19937& random);

/** @returns A particle born in a group: at a point of the area of
 * @p description, with a speed, an angle, a drift, a life and a sprite drawn
 * from @p random in what it describes. */
[[nodiscard]] Particle emittedParticle(const ParticleContent& description,
                                       std::mt19937& random);

/** @returns A height of the area of @p description, drawn from @p random:
 * where a particle of a population comes back. */
[[nodiscard]] float drawnHeight(const ParticleContent& description,
                                std::mt19937& random);

/** @brief Moves @p particle, born in a group, on by @p seconds: its place,
 * its age, the frame of its sprite, and its trail, which grows while it
 * lives and shrinks afterwards. */
void moveParticle(Particle& particle, const ParticleContent& description,
                  float seconds);

/**
 * @returns Whether @p particle, born in a group, has ended: once its life
 * and its trail are over, for one that has a life; once it is more than the
 * margin of @p description out of the background, for one that has none.
 * @param column The column the origin of its layer lies on.
 */
[[nodiscard]] bool hasEnded(const Particle& particle,
                            const ParticleContent& description, int column);

}  // namespace rtype::client
