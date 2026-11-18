#include <gtest/gtest.h>
#include "LevelTransitionScene.hpp"
#include "LevelTransitionTestConstants.hpp"

using rtype::client::LevelTransitionScene;

/**
 * Given a status that tells a transition with 2500 ms left
 * When the scene shows it
 * Then it has 2500 ms left and holds the ships
 */
TEST(LevelTransitionScene, AToldTransitionHoldsTheShips) {
  LevelTransitionScene scene;

  scene.show(TOLD_TRANSITION_LEFT);

  EXPECT_EQ(scene.left(), TOLD_TRANSITION_DURATION);
  EXPECT_TRUE(scene.holdsTheShips());
}

/**
 * Given a transition with 2500 ms left
 * When 3000 ms pass
 * Then it ended: no time left, and the ships are no longer held
 */
TEST(LevelTransitionScene, ATransitionEndsOnceItsTimePassed) {
  LevelTransitionScene scene;
  scene.show(TOLD_TRANSITION_LEFT);

  scene.advance(LONGER_THAN_THE_TOLD_TRANSITION);

  EXPECT_EQ(scene.left(), NO_TIME_LEFT);
  EXPECT_FALSE(scene.holdsTheShips());
}

/**
 * Given a transition with 2500 ms left
 * When a status that tells no transition comes
 * Then it ends at once
 */
TEST(LevelTransitionScene, AStatusWithoutTransitionEndsIt) {
  LevelTransitionScene scene;
  scene.show(TOLD_TRANSITION_LEFT);

  scene.show(NO_TRANSITION_TOLD);

  EXPECT_FALSE(scene.holdsTheShips());
}
