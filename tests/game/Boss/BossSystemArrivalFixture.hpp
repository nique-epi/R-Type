#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#include "BossArrival.hpp"
#include "BossProgress.hpp"
#include "BossSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EventBus.hpp"
#include "Health.hpp"

namespace rtype::game::testing {

/**
 * @brief A boss just created, which a BossSystem warns the players of then
 * wakes up, for one connected player and a plain difficulty unless a test
 * says otherwise; the warnings it publishes are recorded once delivered.
 */
class BossSystemArrivalFixture : public ::testing::Test {
 protected:
  BossSystemArrivalFixture();

  /** @brief Builds the system again, for a difficulty that gives every boss
   * @p healthPercent percent of its health. */
  void useHealthPercent(int healthPercent);

  /** @brief Makes @p players players connected from now on. */
  void connect(std::size_t players);

  /** @brief Adds the boss ARRIVING_BOSS_ID, as the world creates it. */
  void addNewBoss();

  /** @brief Runs the system once, then delivers what it published. */
  void run();

  /** @brief Ends the warning of the boss, as SummoningSystem does once it
   * counted its Summoning down. */
  void endTheWarning();

  [[nodiscard]] const std::vector<BossArrival>& arrivals() const;

  /** @returns The ticks left of the Summoning of the boss; nothing when it
   * has none. */
  [[nodiscard]] std::optional<std::size_t> summoningTicksLeft();

  /** @returns The attack the boss is armored in; nothing when it has no
   * Armored. */
  [[nodiscard]] std::optional<std::uint8_t> armoredAttack();

  /** @returns The health of the boss; none when it has no Health. */
  [[nodiscard]] Health health();

  /** @returns How far the boss is; Created when it has no Boss. */
  [[nodiscard]] BossProgress progress();

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  engine::EventBus events_;
  std::size_t players_{1};
  std::unique_ptr<BossSystem> system_;
  engine::Entity boss_{};
  std::vector<BossArrival> arrivals_;
};

}  // namespace rtype::game::testing
