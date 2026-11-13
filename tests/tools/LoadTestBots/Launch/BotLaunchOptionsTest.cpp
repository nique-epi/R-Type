#include <gtest/gtest.h>
#include "BotLaunch.hpp"
#include "BotLaunchOptions.hpp"
#include "BotLaunchOptionsTestConstants.hpp"
#include "BotPilotKind.hpp"
#include "EngineException.hpp"

using rtype::engine::InvalidLaunchOptionValueException;
using rtype::tools::BotLaunch;
using rtype::tools::BotPilotKind;
using rtype::tools::readBotLaunch;
using rtype::tools::testing::A_RUN_OF_TWO_SECONDS;
using rtype::tools::testing::ANOTHER_SEED;
using rtype::tools::testing::ANOTHER_SEED_VALUE;
using rtype::tools::testing::ANOTHER_SERVER;
using rtype::tools::testing::ANOTHER_SERVER_ADDRESS;
using rtype::tools::testing::ANOTHER_SERVER_PORT;
using rtype::tools::testing::FOUR_FULL_GAMES;
using rtype::tools::testing::LARGEST_SQUAD;
using rtype::tools::testing::LARGEST_SQUAD_SIZE;
using rtype::tools::testing::NO_BOT;
using rtype::tools::testing::NO_SECOND;
using rtype::tools::testing::ONE_BOT;
using rtype::tools::testing::ONE_BOT_TOO_MANY;
using rtype::tools::testing::TWO_SECONDS;
using rtype::tools::testing::WITH_RULES;

/**
 * Given no argument
 * When the tool reads its launch
 * Then four full games of bots join
 */
TEST(BotLaunchOptions, FourFullGamesJoinByDefault) {
  const BotLaunch launch = readBotLaunch({});

  EXPECT_EQ(launch.botCount, FOUR_FULL_GAMES);
}

/**
 * Given a count of no bot
 * When the tool reads its launch
 * Then the count is refused
 */
TEST(BotLaunchOptions, RefusesASquadOfNoBot) {
  EXPECT_THROW(static_cast<void>(readBotLaunch(NO_BOT)),
               InvalidLaunchOptionValueException);
}

/**
 * Given a count of one bot
 * When the tool reads its launch
 * Then one bot joins
 */
TEST(BotLaunchOptions, AcceptsASquadOfOneBot) {
  const BotLaunch launch = readBotLaunch(ONE_BOT);

  EXPECT_EQ(launch.botCount, 1U);
}

/**
 * Given a count of the largest squad
 * When the tool reads its launch
 * Then that many bots join
 */
TEST(BotLaunchOptions, AcceptsTheLargestSquad) {
  const BotLaunch launch = readBotLaunch(LARGEST_SQUAD);

  EXPECT_EQ(launch.botCount, LARGEST_SQUAD_SIZE);
}

/**
 * Given a count one bot above the largest squad
 * When the tool reads its launch
 * Then the count is refused
 */
TEST(BotLaunchOptions, RefusesOneBotTooMany) {
  EXPECT_THROW(static_cast<void>(readBotLaunch(ONE_BOT_TOO_MANY)),
               InvalidLaunchOptionValueException);
}

/**
 * Given a run of no second
 * When the tool reads its launch
 * Then the duration is refused
 */
TEST(BotLaunchOptions, RefusesARunOfNoSecond) {
  EXPECT_THROW(static_cast<void>(readBotLaunch(NO_SECOND)),
               InvalidLaunchOptionValueException);
}

/**
 * Given a run of two seconds
 * When the tool reads its launch
 * Then the bots play for two seconds
 */
TEST(BotLaunchOptions, PlaysForTheDurationItIsGiven) {
  const BotLaunch launch = readBotLaunch(A_RUN_OF_TWO_SECONDS);

  EXPECT_EQ(launch.duration, TWO_SECONDS);
}

/**
 * Given a seed other than the default one
 * When the tool reads its launch
 * Then the bots draw from that seed
 */
TEST(BotLaunchOptions, DrawsFromTheSeedItIsGiven) {
  const BotLaunch launch = readBotLaunch(ANOTHER_SEED);

  EXPECT_EQ(launch.seed, ANOTHER_SEED_VALUE);
}

/**
 * Given the address and the port of another server
 * When the tool reads its launch
 * Then the bots join that server
 */
TEST(BotLaunchOptions, JoinsTheServerItIsGiven) {
  const BotLaunch launch = readBotLaunch(ANOTHER_SERVER);

  EXPECT_EQ(launch.server.address, ANOTHER_SERVER_ADDRESS);
  EXPECT_EQ(launch.server.port, ANOTHER_SERVER_PORT);
}

/**
 * Given no option about the pilot
 * When the tool reads its launch
 * Then the bots fly at random
 */
TEST(BotLaunchOptions, BotsFlyAtRandomByDefault) {
  EXPECT_EQ(readBotLaunch({}).pilot, BotPilotKind::Random);
}

/**
 * Given the option of the rules
 * When the tool reads its launch
 * Then the rules fly the bots
 */
TEST(BotLaunchOptions, TheRulesCanFlyTheBots) {
  EXPECT_EQ(readBotLaunch(WITH_RULES).pilot, BotPilotKind::Rules);
}
