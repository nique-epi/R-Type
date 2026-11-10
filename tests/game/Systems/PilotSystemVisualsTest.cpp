#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ActionRequestingBehaviour.hpp"
#include "EffectStarted.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
#include "PilotSystemFixture.hpp"
#include "PilotSystemVisualsTestConstants.hpp"
#include "SheetVisualNames.hpp"
#include "ShownLook.hpp"
#include "TimeConstants.hpp"
#include "VisualNames.hpp"

using rtype::engine::Entity;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::EffectStarted;
using rtype::game::Intent;
using rtype::game::SheetVisualNames;
using rtype::game::ShownLook;
using rtype::game::VisualNames;
using rtype::game::testing::ActionRequestingBehaviour;
using rtype::game::testing::ANGRY_LOOK;
using rtype::game::testing::CRACKED_LOOK;
using rtype::game::testing::CRACKED_LOOK_NUMBER;
using rtype::game::testing::IDLE_LOOK;
using rtype::game::testing::IDLE_LOOK_NUMBER;
using rtype::game::testing::PilotSystemFixture;
using rtype::game::testing::SPARK_EFFECT;
using rtype::game::testing::UNNAMED_EFFECT;
using rtype::game::testing::UNNAMED_LOOK;
using rtype::game::testing::VISUAL_BODY_NETWORK_ID;

/**
 * Given a body whose sheet names the effect spark, and whose behaviour plays
 * it
 * When the pilots run once and the events are delivered
 * Then one EffectStarted names the network id of the body and spark
 */
TEST_F(PilotSystemFixture, AnEffectTheSheetNamesIsPublished) {
  const SheetVisualNames names{.effects = {std::string{SPARK_EFFECT}},
                               .looks = {}};
  const ActionRequestingBehaviour behaviour{
      Intent{.playsEffect = std::string{SPARK_EFFECT}}};
  const Entity body = addEnemy(behaviour, VISUAL_BODY_NETWORK_ID);
  components().add(body, VisualNames{.names = &names});

  run(SIMULATION_TICK_DURATION);

  const std::vector<EffectStarted> expected{
      {.networkId = VISUAL_BODY_NETWORK_ID,
       .effect = std::string{SPARK_EFFECT}}};
  EXPECT_EQ(takeStartedEffects(), expected);
}

/**
 * Given a body whose sheet names spark only, and whose behaviour plays smoke
 * When the pilots run once and the events are delivered
 * Then nothing is published
 */
TEST_F(PilotSystemFixture, AnEffectTheSheetDoesNotNameIsDropped) {
  const SheetVisualNames names{.effects = {std::string{SPARK_EFFECT}},
                               .looks = {}};
  const ActionRequestingBehaviour behaviour{
      Intent{.playsEffect = std::string{UNNAMED_EFFECT}}};
  const Entity body = addEnemy(behaviour, VISUAL_BODY_NETWORK_ID);
  components().add(body, VisualNames{.names = &names});

  run(SIMULATION_TICK_DURATION);

  EXPECT_TRUE(takeStartedEffects().empty());
}

/**
 * Given a body whose sheet names the looks angry and cracked, and whose
 * behaviour shows cracked
 * When the pilots run once
 * Then the body shows look 2, the place of cracked in the order of the names
 */
TEST_F(PilotSystemFixture, ALookTheSheetNamesIsShownByItsNumber) {
  const SheetVisualNames names{
      .effects = {},
      .looks = {std::string{ANGRY_LOOK}, std::string{CRACKED_LOOK}}};
  const ActionRequestingBehaviour behaviour{
      Intent{.showsLook = std::string{CRACKED_LOOK}}};
  const Entity body = addEnemy(behaviour, VISUAL_BODY_NETWORK_ID);
  components().add(body, VisualNames{.names = &names});
  components().add(body, ShownLook{});

  run(SIMULATION_TICK_DURATION);

  EXPECT_EQ(components().get<ShownLook>(body)->number, CRACKED_LOOK_NUMBER);
}

/**
 * Given a body showing cracked, whose behaviour then asks for a look its sheet
 * does not name
 * When the pilots run once
 * Then the body keeps showing cracked
 */
TEST_F(PilotSystemFixture, ALookTheSheetDoesNotNameKeepsTheShownOne) {
  const SheetVisualNames names{
      .effects = {},
      .looks = {std::string{ANGRY_LOOK}, std::string{CRACKED_LOOK}}};
  const ActionRequestingBehaviour behaviour{
      Intent{.showsLook = std::string{UNNAMED_LOOK}}};
  const Entity body = addEnemy(behaviour, VISUAL_BODY_NETWORK_ID);
  components().add(body, VisualNames{.names = &names});
  components().add(body, ShownLook{.number = CRACKED_LOOK_NUMBER});

  run(SIMULATION_TICK_DURATION);

  EXPECT_EQ(components().get<ShownLook>(body)->number, CRACKED_LOOK_NUMBER);
}

/**
 * Given a body showing cracked, whose behaviour asks for the empty look
 * When the pilots run once
 * Then the body shows look 0, its own idle loop
 */
TEST_F(PilotSystemFixture, TheEmptyLookGivesTheIdleLoopBack) {
  const SheetVisualNames names{
      .effects = {},
      .looks = {std::string{ANGRY_LOOK}, std::string{CRACKED_LOOK}}};
  const ActionRequestingBehaviour behaviour{
      Intent{.showsLook = std::string{IDLE_LOOK}}};
  const Entity body = addEnemy(behaviour, VISUAL_BODY_NETWORK_ID);
  components().add(body, VisualNames{.names = &names});
  components().add(body, ShownLook{.number = CRACKED_LOOK_NUMBER});

  run(SIMULATION_TICK_DURATION);

  EXPECT_EQ(components().get<ShownLook>(body)->number, IDLE_LOOK_NUMBER);
}
