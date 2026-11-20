#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EntityCuesFixture.hpp"
#include "EntityCuesTestConstants.hpp"

using rtype::client::testing::CUED_APPEAR_SOUND;
using rtype::client::testing::CUED_DEATH_SOUND;
using ::testing::ElementsAre;

/**
 * Given a Bydo whose sheet names the sound of its death
 * When it is killed
 * Then that sound plays, in place of the small explosion of every Bydo
 */
TEST_F(EntityCuesFixture, AKilledBydoPlaysTheDeathItsSheetNames) {
  killTheBydo();

  EXPECT_THAT(played(), ElementsAre(CUED_DEATH_SOUND));
}

/**
 * Given a Bydo whose sheet names the sound of its appearance
 * When it appears
 * Then that sound plays, where a Bydo plays none
 */
TEST_F(EntityCuesFixture, ABydoAppearingPlaysTheSoundItsSheetNames) {
  announceTheBydo();

  EXPECT_THAT(played(), ElementsAre(CUED_APPEAR_SOUND));
}
