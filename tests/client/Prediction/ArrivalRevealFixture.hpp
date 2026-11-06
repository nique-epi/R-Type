#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Effects/EffectTestConstants.hpp"
#include "EntityFeedback.hpp"
#include "EntityRegistry.hpp"
#include "EntityReplicator.hpp"
#include "EntityReplicatorParts.hpp"
#include "FixedEntityTypeSource.hpp"
#include "InterpolationClock.hpp"
#include "Position.hpp"
#include "RecordingSoundPlayer.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

/**
 * @brief The part of a game screen that turns what the server announces into
 * what the player sees: the replicator, the interpolation clock, and each
 * frame placing the entities then revealing those that arrived, in the order
 * of GameScreen::update(). The tests follow one missile, REVEALED_MISSILE_ID.
 */
class ArrivalRevealFixture : public ::testing::Test {
 protected:
  /** @brief Announces the missile, of the type numbered @p typeNumber, at
   * MISSILE_AT_THE_MUZZLE, then receives
   * the states of FIRST_MISSILE_STATE_TICK and SECOND_MISSILE_STATE_TICK that
   * list it at MISSILE_AT_THE_MUZZLE and MISSILE_THREE_TICKS_LATER: the clock
   * then shows tick 27. */
  void fireTheMissileOf(std::uint8_t typeNumber);

  /** @brief Moves the clock on by one tick, places the entities at the tick it
   * shows, then reveals those that arrived. */
  void showOneMoreFrame();

  /** @returns Whether the missile is drawn, as RenderSystem would draw it. */
  [[nodiscard]] bool missileIsDrawn();

  /** @returns Where the missile is drawn; NO_MISSILE_POSITION while it is
   * not drawn. */
  [[nodiscard]] game::Position missilePosition();

  /** @returns The sounds the replicator and the reveal played. */
  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  /** @brief Hands @p state to the replicator and tells the clock it arrived.
   */
  void receive(const network::WorldState& state);

  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EffectSpawner effects_{entities_, components_, EFFECT_TEST_SEED};
  RecordingSoundPlayer sounds_;
  FixedEntityTypeSource types_;
  EntityFeedback feedback_{.effects = effects_, .sounds = sounds_};
  EntityReplicator replicator_{EntityReplicatorParts{.entities = entities_,
                                                     .components = components_,
                                                     .feedback = feedback_,
                                                     .types = types_}};
  InterpolationClock clock_;
};

}  // namespace rtype::client::testing
