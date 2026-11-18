#pragma once

#include <cstddef>
#include "IParticleEmitter.hpp"

/** @brief Particle emitter that only counts: the births it was asked for,
 * and the particles the test has not ended yet. */
class CountingEmitter final : public rtype::client::IParticleEmitter {
 public:
  void emit() override;
  [[nodiscard]] std::size_t aliveCount() const override;

  /** @returns How many births it was asked for since it was built. */
  [[nodiscard]] std::size_t births() const;

  /** @brief Ends one of its particles, when one lives. */
  void endOne();

  /** @brief Ends every particle that lives. */
  void endAll();

 private:
  std::size_t births_{};
  std::size_t alive_{};
};
