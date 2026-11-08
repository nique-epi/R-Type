#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "ConnectedPlayer.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/** @brief A world and a roster of one player whose ship fired one missile. */
class ShooterViewsFixture : public ::testing::Test {
 protected:
  ShooterViewsFixture();

  /** @returns The player who fired the missile. */
  [[nodiscard]] ConnectedPlayer& shooter();

  /** @brief Gives the missile the view of its player at @p tick. */
  void giveViewsAt(std::uint32_t tick);

  /** @returns The ViewDelay of the missile, in ticks; nothing without one. */
  [[nodiscard]] std::optional<std::size_t> missileDelay();

 private:
  game::World world_;
  PlayerRoster roster_;
  ConnectedPlayer* shooter_{};
};

}  // namespace rtype::server::testing
