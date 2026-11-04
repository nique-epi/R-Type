#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <vector>
#include "DeltaWorldState.hpp"
#include "StateDelta.hpp"
#include "WorldState.hpp"
#include "WorldStateAssembler.hpp"
#include "WorldStateAssemblerFixtures.hpp"
#include "WorldStateAssemblerTestConstants.hpp"

namespace rtype::client {

using network::DeltaWorldState;
using network::EntityState;
using network::StateDelta;
using network::WorldState;
using testing::AFTER_THE_MOVE;
using testing::BEFORE_THE_MOVE;
using testing::bydoAt;
using testing::deltaPart;
using testing::FIRST_ENTITY_ID;
using testing::FIRST_PART;
using testing::FIRST_TICK;
using testing::FIRST_TICK_OF_A_RUN;
using testing::fullPart;
using testing::LAST_TICK_OF_A_FULL_HISTORY;
using testing::LATE_TICK_OF_A_LONG_GAME;
using testing::LONGEST_REACH;
using testing::ONE_PART;
using testing::ONE_TICK_PAST_THE_REACH;
using testing::partOf;
using testing::PartPosition;
using testing::SECOND_ENTITY_ID;
using testing::SECOND_PART;
using testing::SECOND_TICK;
using testing::THIRD_ENTITY_ID;
using testing::THREE_PARTS_COUNT_MISMATCH;
using testing::THREE_TICKS_BACK;
using testing::TICK_AFTER_A_FULL_HISTORY;
using testing::TICK_AFTER_AN_OVERFULL_HISTORY;
using testing::TICK_OF_A_NEW_GAME;
using testing::TWO_PARTS;

/**
 * Given a part that lists every entity, with no reference
 * When it is added
 * Then the state of its tick is rebuilt with those entities and the players
 */
TEST(WorldStateAssembler, RebuildsAStateWrittenWithoutReference) {
  WorldStateAssembler assembler;

  const std::optional<WorldState> state = assembler.add(fullPart(
      FIRST_TICK,
      {bydoAt(FIRST_ENTITY_ID, BEFORE_THE_MOVE), bydoAt(SECOND_ENTITY_ID, 0)}));

  ASSERT_TRUE(state.has_value());
  const WorldState rebuilt = state.value_or(WorldState{});
  EXPECT_EQ(rebuilt.tick, FIRST_TICK);
  EXPECT_EQ(rebuilt.entities,
            (std::vector<EntityState>{bydoAt(FIRST_ENTITY_ID, BEFORE_THE_MOVE),
                                      bydoAt(SECOND_ENTITY_ID, 0)}));
  EXPECT_EQ(rebuilt.players.size(), 1U);
}

/**
 * Given a rebuilt state of two entities
 * When a delta against it moves the first, removes the second and adds a third
 * Then the state of the delta lists the moved first and the third
 */
TEST(WorldStateAssembler, RebuildsAStateWrittenAgainstAnEarlierOne) {
  WorldStateAssembler assembler;
  ASSERT_TRUE(
      assembler
          .add(fullPart(FIRST_TICK, {bydoAt(FIRST_ENTITY_ID, BEFORE_THE_MOVE),
                                     bydoAt(SECOND_ENTITY_ID, 0)}))
          .has_value());

  const std::optional<WorldState> state = assembler.add(
      deltaPart(SECOND_TICK, THREE_TICKS_BACK,
                StateDelta{.changed = {bydoAt(FIRST_ENTITY_ID, AFTER_THE_MOVE),
                                       bydoAt(THIRD_ENTITY_ID, 0)},
                           .removed = {SECOND_ENTITY_ID}}));

  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state.value_or(WorldState{}).entities,
            (std::vector<EntityState>{bydoAt(FIRST_ENTITY_ID, AFTER_THE_MOVE),
                                      bydoAt(THIRD_ENTITY_ID, 0)}));
}

/**
 * Given a state of two parts, the second arriving first
 * When both are added
 * Then nothing is rebuilt after the first, and the state lists the entities of
 * both parts once the second has arrived
 */
TEST(WorldStateAssembler, WaitsForEveryPartOfATick) {
  WorldStateAssembler assembler;
  const DeltaWorldState whole =
      fullPart(FIRST_TICK, {bydoAt(FIRST_ENTITY_ID, 0)});
  DeltaWorldState other = fullPart(FIRST_TICK, {bydoAt(SECOND_ENTITY_ID, 0)});

  const std::optional<WorldState> afterOne =
      assembler.add(partOf(other, PartPosition{SECOND_PART, TWO_PARTS}));
  const std::optional<WorldState> afterBoth =
      assembler.add(partOf(whole, PartPosition{FIRST_PART, TWO_PARTS}));

  EXPECT_FALSE(afterOne.has_value());
  ASSERT_TRUE(afterBoth.has_value());
  EXPECT_EQ(afterBoth.value_or(WorldState{}).entities,
            (std::vector<EntityState>{bydoAt(FIRST_ENTITY_ID, 0),
                                      bydoAt(SECOND_ENTITY_ID, 0)}));
}

/**
 * Given a state of two parts whose first part arrived twice
 * When the second part has not arrived
 * Then nothing is rebuilt: a part counts once
 */
TEST(WorldStateAssembler, CountsAPartOnceWhenItArrivesTwice) {
  WorldStateAssembler assembler;
  const DeltaWorldState first =
      partOf(fullPart(FIRST_TICK, {bydoAt(FIRST_ENTITY_ID, 0)}),
             PartPosition{FIRST_PART, TWO_PARTS});

  EXPECT_FALSE(assembler.add(first).has_value());
  EXPECT_FALSE(assembler.add(first).has_value());
}

/**
 * Given a part that says another part count than the first part of its tick
 * When it is added after that first part
 * Then it is left out, and the state is not rebuilt from the two
 */
TEST(WorldStateAssembler, LeavesOutAPartThatDisagreesOnThePartCount) {
  WorldStateAssembler assembler;
  ASSERT_FALSE(assembler
                   .add(partOf(fullPart(FIRST_TICK, {}),
                               PartPosition{FIRST_PART, TWO_PARTS}))
                   .has_value());

  EXPECT_FALSE(
      assembler
          .add(partOf(fullPart(FIRST_TICK, {}),
                      PartPosition{SECOND_PART, THREE_PARTS_COUNT_MISMATCH}))
          .has_value());
}

/**
 * Given a tick whose second part never arrived
 * When a part of a newer tick arrives
 * Then the newer tick is rebuilt, and the older one never is
 */
TEST(WorldStateAssembler, DropsAnIncompleteTickWhenANewerOneComes) {
  WorldStateAssembler assembler;
  ASSERT_FALSE(assembler
                   .add(partOf(fullPart(FIRST_TICK, {}),
                               PartPosition{FIRST_PART, TWO_PARTS}))
                   .has_value());

  const std::optional<WorldState> state =
      assembler.add(fullPart(SECOND_TICK, {bydoAt(FIRST_ENTITY_ID, 0)}));

  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state.value_or(WorldState{}).tick, SECOND_TICK);
}

/**
 * Given nothing rebuilt yet
 * When a delta against an earlier tick arrives
 * Then no state is rebuilt, since its reference is not held
 */
TEST(WorldStateAssembler, DropsAStateWhoseReferenceIsNotHeld) {
  WorldStateAssembler assembler;

  EXPECT_FALSE(
      assembler.add(deltaPart(SECOND_TICK, THREE_TICKS_BACK, StateDelta{}))
          .has_value());
}

/**
 * Given 60 states rebuilt from tick 1 to tick 60
 * When a delta of tick 61 reaches back 60 ticks, to the first
 * Then the state is rebuilt
 */
TEST(WorldStateAssembler, HoldsTheReferenceAtTheLongestReach) {
  WorldStateAssembler assembler;
  for (std::uint32_t tick = FIRST_TICK_OF_A_RUN;
       tick <= LAST_TICK_OF_A_FULL_HISTORY; ++tick) {
    ASSERT_TRUE(assembler.add(fullPart(tick, {})).has_value());
  }

  EXPECT_TRUE(assembler
                  .add(deltaPart(TICK_AFTER_A_FULL_HISTORY, LONGEST_REACH,
                                 StateDelta{}))
                  .has_value());
}

/**
 * Given 61 states rebuilt from tick 1 to tick 61, so the first was dropped
 * When a delta of tick 62 reaches back 61 ticks, to the first
 * Then no state is rebuilt
 */
TEST(WorldStateAssembler, DropsTheOldestStateBeyondTheHistory) {
  WorldStateAssembler assembler;
  for (std::uint32_t tick = FIRST_TICK_OF_A_RUN;
       tick <= TICK_AFTER_A_FULL_HISTORY; ++tick) {
    ASSERT_TRUE(assembler.add(fullPart(tick, {})).has_value());
  }

  EXPECT_FALSE(assembler
                   .add(deltaPart(TICK_AFTER_AN_OVERFULL_HISTORY,
                                  ONE_TICK_PAST_THE_REACH, StateDelta{}))
                   .has_value());
}

/**
 * Given a state rebuilt at tick 100
 * When a state of tick 2 arrives, as when a new game starts
 * Then it is rebuilt: the game started over
 */
TEST(WorldStateAssembler, StartsOverWhenTheTickGoesBack) {
  WorldStateAssembler assembler;
  ASSERT_TRUE(
      assembler.add(fullPart(LATE_TICK_OF_A_LONG_GAME, {})).has_value());

  const std::optional<WorldState> state =
      assembler.add(fullPart(TICK_OF_A_NEW_GAME, {bydoAt(FIRST_ENTITY_ID, 0)}));

  ASSERT_TRUE(state.has_value());
  EXPECT_EQ(state.value_or(WorldState{}).tick, TICK_OF_A_NEW_GAME);
}

/**
 * Given a state rebuilt at tick 100, then a state of tick 2 of a new game
 * When a delta reaches back to tick 100
 * Then no state is rebuilt: the old game's states were forgotten
 */
TEST(WorldStateAssembler, ForgetsTheStatesOfTheGameBeforeWhenItStartsOver) {
  WorldStateAssembler assembler;
  ASSERT_TRUE(
      assembler.add(fullPart(LATE_TICK_OF_A_LONG_GAME, {})).has_value());
  ASSERT_TRUE(assembler.add(fullPart(TICK_OF_A_NEW_GAME, {})).has_value());

  EXPECT_FALSE(
      assembler
          .add(deltaPart(LATE_TICK_OF_A_LONG_GAME + 1, ONE_PART, StateDelta{}))
          .has_value());
}

/**
 * Given a state rebuilt at tick 3
 * When the assembler is cleared, then a delta against tick 3 arrives
 * Then no state is rebuilt
 */
TEST(WorldStateAssembler, ForgetsWhatItHoldsWhenCleared) {
  WorldStateAssembler assembler;
  ASSERT_TRUE(assembler.add(fullPart(FIRST_TICK, {})).has_value());

  assembler.clear();

  EXPECT_FALSE(
      assembler.add(deltaPart(SECOND_TICK, THREE_TICKS_BACK, StateDelta{}))
          .has_value());
}

}  // namespace rtype::client
