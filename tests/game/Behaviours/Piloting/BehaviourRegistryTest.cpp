#include <gtest/gtest.h>
#include <memory>
#include <string_view>
#include <vector>
#include "BehaviourConstants.hpp"
#include "BehaviourRegistry.hpp"
#include "BehaviourRegistryTestConstants.hpp"
#include "BuiltInBehaviours.hpp"
#include "GameException.hpp"
#include "StraightBehaviour.hpp"

using rtype::game::BehaviourRegistry;
using rtype::game::builtInBehaviours;
using rtype::game::DuplicateBehaviourException;
using rtype::game::StraightBehaviour;
using rtype::game::UnknownBehaviourException;
using rtype::game::WEAVE_BEHAVIOUR_NAME;
using rtype::game::testing::BUILT_IN_BEHAVIOUR_NAMES;
using rtype::game::testing::UNKNOWN_BEHAVIOUR_NAME;

/**
 * Given the built-in behaviours
 * When the one named weave is looked up
 * Then the behaviour found is named weave
 */
TEST(BehaviourRegistry, FindsABehaviourByItsName) {
  const BehaviourRegistry behaviours = builtInBehaviours();

  EXPECT_EQ(behaviours.named(WEAVE_BEHAVIOUR_NAME).name(),
            WEAVE_BEHAVIOUR_NAME);
}

/**
 * Given the built-in behaviours
 * When a name none of them has is looked up
 * Then it is refused, so a body is never given a behaviour that does not exist
 */
TEST(BehaviourRegistry, RefusesANameNoBehaviourHas) {
  const BehaviourRegistry behaviours = builtInBehaviours();

  EXPECT_THROW(static_cast<void>(behaviours.named(UNKNOWN_BEHAVIOUR_NAME)),
               UnknownBehaviourException);
}

/**
 * Given a registry holding a straight behaviour
 * When a second straight behaviour is added
 * Then it is refused: one name finds one behaviour
 */
TEST(BehaviourRegistry, RefusesASecondBehaviourOfTheSameName) {
  BehaviourRegistry behaviours;
  behaviours.add(std::make_unique<StraightBehaviour>());

  EXPECT_THROW(behaviours.add(std::make_unique<StraightBehaviour>()),
               DuplicateBehaviourException);
}

/**
 * Given the built-in behaviours
 * When a name none of them has is searched for
 * Then nothing is found, and nothing is thrown: the reader of a sheet names
 * the file in its own error
 */
TEST(BehaviourRegistry, FindsNothingForANameNoBehaviourHas) {
  const BehaviourRegistry behaviours = builtInBehaviours();

  EXPECT_EQ(behaviours.find(UNKNOWN_BEHAVIOUR_NAME), nullptr);
}

/**
 * Given the built-in behaviours
 * When their names are asked for
 * Then they are straight then weave, the order they were added in
 */
TEST(BehaviourRegistry, NamesEveryBehaviourInTheOrderTheyWereAdded) {
  const BehaviourRegistry behaviours = builtInBehaviours();

  EXPECT_EQ(behaviours.names(),
            (std::vector<std::string_view>{BUILT_IN_BEHAVIOUR_NAMES.begin(),
                                           BUILT_IN_BEHAVIOUR_NAMES.end()}));
}
