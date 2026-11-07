#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include "ConnectionConstants.hpp"
#include "GameStarted.hpp"
#include "LookItemWon.hpp"
#include "PlayerLooks.hpp"
#include "ServerSessionTestConstants.hpp"
#include "ShipLook.hpp"
#include "ShipLookChoice.hpp"
#include "ShipLookExchangeFixture.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::client {

using identifiers::ShipLook;
using testing::ACCEPTED_PLAYER_NUMBER;
using ::testing::ElementsAre;
using testing::FIRST_SEQUENCE;
using ::testing::IsEmpty;
using testing::SECOND_SEQUENCE;
using testing::ShipLookExchangeFixture;

namespace {

constexpr ShipLook PARTY_HAT_LOOK{.head = 2};
constexpr std::uint8_t FIRST_WON_ITEM = 3;
constexpr std::uint8_t SECOND_WON_ITEM = 8;

}  // namespace

/**
 * Given a session the server accepted
 * When the player chooses a look
 * Then a ShipLookChoice goes out at once
 */
TEST_F(ShipLookExchangeFixture, SendsTheChosenLookAtOnce) {
  accept();

  shipLooks().chooseLook(PARTY_HAT_LOOK);

  EXPECT_EQ(sentCountOf<network::ShipLookChoice>(), 1U);
}

/**
 * Given a look chosen that no PlayerLooks shows
 * When one interval passes
 * Then the ShipLookChoice is sent again, since UDP may have lost it
 */
TEST_F(ShipLookExchangeFixture, SendsItAgainAfterTheIntervalWhileNotShown) {
  accept();
  shipLooks().chooseLook(PARTY_HAT_LOOK);

  update(SHIP_LOOK_CHOICE_INTERVAL);

  EXPECT_EQ(sentCountOf<network::ShipLookChoice>(), 2U);
}

/**
 * Given a look chosen that no PlayerLooks shows
 * When one interval less a nanosecond passes
 * Then it is not sent again yet
 */
TEST_F(ShipLookExchangeFixture, WaitsOneIntervalBeforeSendingAgain) {
  accept();
  shipLooks().chooseLook(PARTY_HAT_LOOK);

  update(SHIP_LOOK_CHOICE_INTERVAL - ONE_NANOSECOND);

  EXPECT_EQ(sentCountOf<network::ShipLookChoice>(), 1U);
}

/**
 * Given a look chosen
 * When a PlayerLooks shows it on the player, then two intervals pass
 * Then the ShipLookChoice is not sent again
 */
TEST_F(ShipLookExchangeFixture, StopsOncePlayerLooksShowsIt) {
  accept();
  shipLooks().chooseLook(PARTY_HAT_LOOK);
  deliverInSession(
      network::PlayerLooks{.players = {{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                        .look = PARTY_HAT_LOOK}}},
      FIRST_SEQUENCE);
  update();

  update(2 * SHIP_LOOK_CHOICE_INTERVAL);

  EXPECT_EQ(sentCountOf<network::ShipLookChoice>(), 1U);
}

/**
 * Given a session where the game started
 * When the player chooses a look
 * Then nothing is sent
 */
TEST_F(ShipLookExchangeFixture, SendsNothingOnceTheGameStarted) {
  accept();
  deliverInSession(network::GameStarted{}, FIRST_SEQUENCE);
  update();

  shipLooks().chooseLook(PARTY_HAT_LOOK);

  EXPECT_EQ(sentCountOf<network::ShipLookChoice>(), 0U);
}

/**
 * Given a look already asked for
 * When the player chooses the same look again
 * Then nothing more is sent
 */
TEST_F(ShipLookExchangeFixture, SendsNothingForTheLookAlreadyAsked) {
  accept();
  shipLooks().chooseLook(PARTY_HAT_LOOK);

  shipLooks().chooseLook(PARTY_HAT_LOOK);

  EXPECT_EQ(sentCountOf<network::ShipLookChoice>(), 1U);
}

/**
 * Given a session that listed the looks of two pilots
 * When the player leaves it
 * Then no look is kept
 */
TEST_F(ShipLookExchangeFixture, ForgetsTheLooksWhenTheSessionCloses) {
  accept();
  deliverInSession(
      network::PlayerLooks{.players = {{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                        .look = PARTY_HAT_LOOK}}},
      FIRST_SEQUENCE);
  update();
  ASSERT_EQ(shipLooks().playerLooks().size(), 1U);

  session().cancel();
  update();

  EXPECT_THAT(shipLooks().playerLooks(), IsEmpty());
}

/**
 * Given a session
 * When the server sends two LookItemWon
 * Then takeWonItems() hands both over in the order they came, then none
 */
TEST_F(ShipLookExchangeFixture, KeepsTheWonItemsUntilTaken) {
  accept();
  deliverInSession(network::LookItemWon{.item = FIRST_WON_ITEM},
                   FIRST_SEQUENCE);
  deliverInSession(network::LookItemWon{.item = SECOND_WON_ITEM},
                   SECOND_SEQUENCE);
  update();

  EXPECT_THAT(shipLooks().takeWonItems(),
              ElementsAre(FIRST_WON_ITEM, SECOND_WON_ITEM));
  EXPECT_THAT(shipLooks().takeWonItems(), IsEmpty());
}

/**
 * Given a session
 * When a PlayerLooks lists two pilots
 * Then the exchange lists them in the order of the message
 */
TEST_F(ShipLookExchangeFixture, KeepsTheLooksOfTheNewestPlayerLooks) {
  accept();

  deliverInSession(
      network::PlayerLooks{
          .players = {{.playerNumber = 1, .look = {}},
                      {.playerNumber = 2, .look = PARTY_HAT_LOOK}}},
      FIRST_SEQUENCE);
  update();

  EXPECT_THAT(shipLooks().playerLooks(),
              ElementsAre(network::PlayerLook{.playerNumber = 1, .look = {}},
                          network::PlayerLook{.playerNumber = 2,
                                              .look = PARTY_HAT_LOOK}));
}

}  // namespace rtype::client
