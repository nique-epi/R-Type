#pragma once

#include <cstddef>

namespace rtype::client {

/** @brief A layer of particles an emission group gives birth in: the group
 * asks for a particle, and counts the ones that live. */
class IParticleEmitter {
 public:
  virtual ~IParticleEmitter() = default;

  /** @brief A particle is born. */
  virtual void emit() = 0;

  /** @returns How many particles live. */
  [[nodiscard]] virtual std::size_t aliveCount() const = 0;
};

}  // namespace rtype::client
