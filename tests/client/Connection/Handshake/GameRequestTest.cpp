#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "ConnectionRequest.hpp"
#include "GameRequest.hpp"
#include "GameRequestTestConstants.hpp"
#include "GameRules.hpp"
#include "GameRulesTestConstants.hpp"

using rtype::client::GameRequest;
using rtype::client::gameRequestFor;
using rtype::client::rulesSentFor;
using rtype::client::testing::NAMED_GAME;
using rtype::network::ANY_GAME_CHOICE;
using rtype::network::defaultGameRules;
using rtype::network::GameChoice;
using rtype::network::GameChoiceKind;
using rtype::network::GameRules;
using rtype::network::NEW_GAME_CHOICE;
using rtype::testing::CHOSEN_DIFFICULTY;
using rtype::testing::CHOSEN_LIVES;

/**
 * Given the choice of any game
 * When the client builds its request
 * Then it knows the rules of that game: the default ones, the only ones a
 * server places such a player in
 */
TEST(GameRequest, AnyGameIsPlayedByTheDefaultRules) {
  EXPECT_EQ(gameRequestFor(ANY_GAME_CHOICE).rules, defaultGameRules());
}

/**
 * Given a game named by its number at launch
 * When the client builds its request
 * Then it knows no rules for it
 */
TEST(GameRequest, ANamedGameHasNoKnownRules) {
  const GameChoice named{.kind = GameChoiceKind::ExistingGame,
                         .number = NAMED_GAME};

  EXPECT_FALSE(gameRequestFor(named).rules.has_value());
}

/**
 * Given a request for a new game with rules the player chose
 * When the rules it sends are read
 * Then they are the chosen rules
 */
TEST(GameRequest, ANewGameSendsTheChosenRules) {
  const GameRules chosen{.difficulty = std::string(CHOSEN_DIFFICULTY),
                         .lives = CHOSEN_LIVES,
                         .friendlyFire = true};

  EXPECT_EQ(rulesSentFor(GameRequest{.game = NEW_GAME_CHOICE, .rules = chosen}),
            chosen);
}

/**
 * Given a request for a new game with no rules
 * When the rules it sends are read
 * Then they are the default rules, as a NewGame always carries rules
 */
TEST(GameRequest, ANewGameWithoutRulesSendsTheDefaultOnes) {
  EXPECT_EQ(
      rulesSentFor(GameRequest{.game = NEW_GAME_CHOICE, .rules = std::nullopt}),
      defaultGameRules());
}

/**
 * Given a request for any game, which knows the default rules
 * When the rules it sends are read
 * Then it sends none, as the protocol requires of any game but a new one
 */
TEST(GameRequest, AnyGameSendsNoRules) {
  EXPECT_FALSE(rulesSentFor(gameRequestFor(ANY_GAME_CHOICE)).has_value());
}
