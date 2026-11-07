#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Effects/EffectTestConstants.hpp"
#include "EntityFeedback.hpp"
#include "EntityRegistry.hpp"
#include "FixedEntityTypeSource.hpp"
#include "OwnMissilesParts.hpp"
#include "OwnPrediction.hpp"
#include "PredictionTestConstants.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "RecordingSoundPlayer.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

/**
 * @brief The prediction of player 2, its gun the gun of the shipped ship (see
 * FixedEntityTypeSource), drawn in registries of its own, after a state that
 * listed its ship.
 */
class OwnPredictionFixture : public ::testing::Test {
 protected:
  OwnPredictionFixture();

  /** @brief Advances the prediction by one tick, holding shoot. */
  void shootOneTick();

  /** @brief Gives the prediction @p state. */
  void apply(const network::WorldState& state);

  /** @returns How many missiles of a player are drawn. */
  [[nodiscard]] std::size_t drawnMissileCount();

  /** @returns The prediction under test. */
  [[nodiscard]] OwnPrediction& prediction();

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  EffectSpawner effects_{entities_, components_, EFFECT_TEST_SEED};
  RecordingSoundPlayer sounds_;
  FixedEntityTypeSource types_;
  RecordingPlayerInputLink link_;
  OwnPrediction prediction_{
      OWN_PLAYER_NUMBER,
      OwnMissilesParts{
          .entities = entities_,
          .components = components_,
          .feedback = EntityFeedback{.effects = effects_, .sounds = sounds_},
          .types = types_}};
};

}  // namespace rtype::client::testing
