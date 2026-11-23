#include <gtest/gtest.h>
#include <cstddef>
#include "IThreatLevel.hpp"
#include "Instances/GameModuleFixture.hpp"
#include "ThreatModule.hpp"
#include "ThreatModuleTestConstants.hpp"

namespace rtype::server::testing {

/**
 * Given a game
 * When the threat module is installed
 * Then the game offers its threat, none yet
 */
TEST_F(GameModuleFixture, ThreatModuleProvidesTheThreatOfTheGame) {
  ThreatModule threat;

  install(threat);

  ASSERT_NE(provided<IThreatLevel>(), nullptr);
  EXPECT_EQ(provided<IThreatLevel>()->level(), NO_THREAT);
}

/**
 * Given the threat module started
 * When it is told for 8 s that the team fires its strongest weapon
 * Then its threat is the most
 */
TEST_F(GameModuleFixture, ThreatModuleFollowsWhatTheTeamFires) {
  ThreatModule threat;
  install(threat);
  threat.start();

  for (std::size_t tick = 0; tick < WHOLE_RISE_TICKS; ++tick) {
    provided<IThreatLevel>()->observe(TEAM_WITH_THE_STRONGEST);
  }

  EXPECT_EQ(provided<IThreatLevel>()->level(), THE_MOST_THREAT);
}

}  // namespace rtype::server::testing
