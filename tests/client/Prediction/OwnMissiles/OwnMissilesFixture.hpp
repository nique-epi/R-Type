#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Effects/EffectTestConstants.hpp"
#include "EntityFeedback.hpp"
#include "EntityRegistry.hpp"
#include "FixedEntityTypeSource.hpp"
#include "OwnMissiles.hpp"
#include "OwnMissilesParts.hpp"
#include "Position.hpp"
#include "RecordingSoundPlayer.hpp"

namespace rtype::client::testing {

/**
 * @brief The missiles of player 1, active, their gun armed with the gun of
 * the shipped ship (see FixedEntityTypeSource), drawn in registries of their
 * own.
 */
class OwnMissilesFixture : public ::testing::Test {
 protected:
  OwnMissilesFixture();

  /** @brief Sends command @p input holding shoot, the ship at @p ship. */
  void fireAt(std::uint32_t input, game::Position ship);

  /** @brief Sends the commands after the newest up to @p input, holding
   * nothing. */
  void holdNothingUpTo(std::uint32_t input);

  /** @returns Where every missile drawn is, in no particular order. */
  [[nodiscard]] std::vector<game::Position> drawnMissiles();

  /** @brief Puts an enemy drawn at @p position, with the box of a Bydo. */
  void enemyAt(game::Position position);

  /** @returns The missiles under test. */
  [[nodiscard]] OwnMissiles& missiles();

  /** @returns The sounds their shots played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EffectSpawner effects_{entities_, components_, EFFECT_TEST_SEED};
  RecordingSoundPlayer sounds_;
  FixedEntityTypeSource types_;
  OwnMissiles missiles_{OwnMissilesParts{
      .entities = entities_,
      .components = components_,
      .feedback = EntityFeedback{.effects = effects_, .sounds = sounds_},
      .types = types_}};
  std::uint32_t newestInput_{0};
};

}  // namespace rtype::client::testing
