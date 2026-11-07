#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Effects/EffectTestConstants.hpp"
#include "EntityFeedback.hpp"
#include "EntityRegistry.hpp"
#include "EntityReplicator.hpp"
#include "EntityReplicatorParts.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "FixedEntityTypeSource.hpp"
#include "RecordingOwnMissileListener.hpp"
#include "RecordingSoundPlayer.hpp"

namespace rtype::client::testing {

/**
 * @brief The registries of a game screen, the spawner of its effects, and the
 * replicator that fills them. The tests read the components through the free
 * functions of EntityReplicatorFixtures.hpp, handing them components().
 */
class EntityReplicatorFixture : public ::testing::Test {
 protected:
  /**
   * @returns The replicator the tests hand their messages to.
   */
  EntityReplicator& replicator();

  /**
   * @returns The components the replicator writes, for the readers of
   * EntityReplicatorFixtures.hpp.
   */
  engine::ComponentRegistry& components();

  /**
   * @brief Announces one entity of each type of the protocol, each with the
   * id and the position its test constants give it: SHIP_ID at
   * SHIP_POSITION, PLAYER_MISSILE_ID at PLAYER_MISSILE_POSITION, BYDO_ID at
   * BYDO_POSITION, ENEMY_MISSILE_ID at ENEMY_MISSILE_POSITION.
   */
  void announceOneOfEachType();

  /**
   * @returns How many entities are alive in the registry, effects included.
   */
  [[nodiscard]] std::size_t entityCount() const;

  /** @returns The sounds the replicator played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

  /** @returns Who the replicator hands the missiles of player OWN_PLAYER to;
   * it predicts none until told to. */
  [[nodiscard]] RecordingOwnMissileListener& ownMissiles();

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EffectSpawner effects_{entities_, components_, EFFECT_TEST_SEED};
  RecordingSoundPlayer sounds_;
  FixedEntityTypeSource types_;
  RecordingOwnMissileListener ownMissiles_;
  EntityReplicator replicator_{EntityReplicatorParts{
      .entities = entities_,
      .components = components_,
      .feedback = EntityFeedback{.effects = effects_, .sounds = sounds_},
      .types = types_,
      .ownPlayerNumber = OWN_PLAYER,
      .ownMissiles = &ownMissiles_}};
};

}  // namespace rtype::client::testing
