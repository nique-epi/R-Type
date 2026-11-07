#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include "ConnectedPlayer.hpp"
#include "IHitArbitration.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world holding the ship of one player and a missile it fired, the
 * client of that player having shown tick 95 when it sent its input applied
 * last.
 */
class HitArbitrationFixture : public ::testing::Test {
 protected:
  HitArbitrationFixture();

  /** @brief Gives the views of @p arbitration at tick 100. */
  void giveViewsOf(const IHitArbitration& arbitration);

  /** @returns The ViewDelay of the ship, in ticks; nothing without one. */
  [[nodiscard]] std::optional<std::size_t> shipDelay();

  /** @returns The ViewDelay of the missile, in ticks; nothing without one. */
  [[nodiscard]] std::optional<std::size_t> missileDelay();

 private:
  game::World world_;
  PlayerRoster roster_;
  ConnectedPlayer* player_{};
};

}  // namespace rtype::server::testing
