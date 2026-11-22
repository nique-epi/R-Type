#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string>
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenTestConstants.hpp"
#include "ShopScreenConstants.hpp"
#include "ShopScreenFixture.hpp"
#include "ShopScreenTestConstants.hpp"
#include "WalletChangeReading.hpp"
#include "WalletRefusal.hpp"

using rtype::client::MASTER_SILENT_STATUS;
using rtype::client::PURCHASE_SOUND_ID;
using rtype::client::WalletChangeReading;
using rtype::client::WalletRefusal;
using rtype::client::testing::A_FORTUNE;
using rtype::client::testing::ALMOST_THE_STAMP_TIME;
using rtype::client::testing::ALREADY_YOURS_LINE;
using rtype::client::testing::ALREADY_YOURS_WORD;
using rtype::client::testing::BOUGHT_PARTY_HAT_LINE;
using rtype::client::testing::BUY_BUTTON_MIDDLE;
using rtype::client::testing::CANCEL_BUTTON_MIDDLE;
using rtype::client::testing::CHEAPEST_HULL_ITEM;
using rtype::client::testing::CROWN_ITEM;
using rtype::client::testing::FIRST_STALL_MIDDLE_OF_THE_SHOP;
using rtype::client::testing::FIRST_TOO_EXPENSIVE_WORD;
using rtype::client::testing::OFF_THE_TICKET;
using rtype::client::testing::ONE_KOPECK_MISSING_LINE;
using rtype::client::testing::ONE_KOPECK_UNDER_THE_PARTY_HAT;
using rtype::client::testing::OWNED_PARTY_HAT_LINE;
using rtype::client::testing::PARTY_HAT_FOR_SALE_LINE;
using rtype::client::testing::PARTY_HAT_ITEM;
using rtype::client::testing::PARTY_HAT_LINE_WORD;
using rtype::client::testing::SECOND_CELL_MIDDLE;
using rtype::client::testing::SECOND_TOO_EXPENSIVE_WORD;
using rtype::client::testing::SHOP_SCENE_SHEET;
using rtype::client::testing::ShopScreenFixture;
using rtype::client::testing::showsATicket;
using rtype::client::testing::THE_REST_OF_THE_STAMP_TIME;
using rtype::client::testing::WAR_SHARK_TOO_EXPENSIVE_WORD;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Not;
using Key = sf::Keyboard::Key;

/**
 * Given a shop just opened, the party hat chosen
 * When it is painted
 * Then the picture of the booth is laid first, and the status line names the
 * party hat and its price
 */
TEST_F(ShopScreenFixture, OpensOnThePictureAndTheFirstWare) {
  ASSERT_FALSE(painting().images.empty());
  EXPECT_EQ(painting().images.front().assetId, SHOP_SCENE_SHEET);
  EXPECT_NE(writingOf(painting(), PARTY_HAT_FOR_SALE_LINE), nullptr);
}

/**
 * Given a rich player, the party hat chosen
 * When Enter is pressed, then BUY on its ticket
 * Then the locker holds the party hat
 */
TEST_F(ShopScreenFixture, EnterPutsTheChosenLookInTheLocker) {
  give(A_FORTUNE);

  buyChosen();

  EXPECT_THAT(wallet().boughtItems, Contains(PARTY_HAT_ITEM));
}

/**
 * Given a rich player, the party hat chosen
 * When Enter is pressed, then BUY on its ticket
 * Then the master is asked to buy it once
 */
TEST_F(ShopScreenFixture, APurchaseAsksTheMasterOnce) {
  give(A_FORTUNE);

  buyChosen();

  EXPECT_EQ(wallet().boughtItems.size(), 1U);
}

/**
 * Given a rich player, the party hat chosen
 * When Enter is pressed, then BUY on its ticket
 * Then the sound of a purchase plays and the status line names the look
 */
TEST_F(ShopScreenFixture, APurchaseIsHeardAndNamed) {
  give(A_FORTUNE);

  buyChosen();

  EXPECT_THAT(sounds().played(), Contains(std::string{PURCHASE_SOUND_ID}));
  EXPECT_NE(writingOf(painting(), BOUGHT_PARTY_HAT_LINE), nullptr);
}

/**
 * Given a player one kopeck short of the party hat
 * When Enter is pressed, then BUY on its ticket
 * Then the status line says one kopeck is missing
 */
TEST_F(ShopScreenFixture, APoorPlayerIsToldWhatIsMissing) {
  give(ONE_KOPECK_UNDER_THE_PARTY_HAT);

  buyChosen();

  EXPECT_NE(writingOf(painting(), ONE_KOPECK_MISSING_LINE), nullptr);
}

/**
 * Given a player one kopeck short of the party hat
 * When Enter is pressed, then BUY on its ticket
 * Then nothing is asked of the master, or heard as a purchase
 */
TEST_F(ShopScreenFixture, ARefusedPurchaseChangesNothing) {
  give(ONE_KOPECK_UNDER_THE_PARTY_HAT);

  buyChosen();

  EXPECT_THAT(wallet().boughtItems, Not(Contains(PARTY_HAT_ITEM)));
  EXPECT_THAT(sounds().played(), Not(Contains(std::string{PURCHASE_SOUND_ID})));
}

/**
 * Given a rich player, the party hat chosen
 * When Enter is held, its press repeated before any release
 * Then its ticket shows and nothing is bought: the held Enter does not press
 * BUY
 */
TEST_F(ShopScreenFixture, AHeldEnterShowsTheTicketAndBuysNothing) {
  give(A_FORTUNE);
  hold(Key::Enter);

  hold(Key::Enter);

  EXPECT_TRUE(showsATicket(painting()));
  EXPECT_THAT(wallet().boughtItems, Not(Contains(PARTY_HAT_ITEM)));
}

/**
 * Given a player who just bought the party hat
 * When Enter is pressed again on it
 * Then the status line says it is already theirs
 */
TEST_F(ShopScreenFixture, BuyingAnOwnedLookSaysItIsAlreadyYours) {
  give(A_FORTUNE);
  buyChosen();

  press(Key::Enter);

  EXPECT_NE(writingOf(painting(), ALREADY_YOURS_LINE), nullptr);
}

/**
 * Given a player who bought the party hat, then chose its neighbour
 * When the party hat is chosen again
 * Then its status line says it is already theirs, and names no price
 */
TEST_F(ShopScreenFixture, AnOwnedLookShowsNoPriceOnItsLine) {
  give(A_FORTUNE);
  buyChosen();
  press(Key::Right);

  press(Key::Left);

  EXPECT_NE(writingOf(painting(), OWNED_PARTY_HAT_LINE), nullptr);
}

/**
 * Given a rich player, on a window whose pixels are not those of the
 * interface
 * When the second cell of the locker is clicked, then BUY on its ticket
 * Then the crown, the look of that cell, is bought
 */
TEST_F(ShopScreenFixture, AClickThenBuyBuysTheLookUnderIt) {
  give(A_FORTUNE);

  pointAt(SECOND_CELL_MIDDLE, true);
  pointAt(BUY_BUTTON_MIDDLE, true);

  EXPECT_THAT(wallet().boughtItems, Contains(CROWN_ITEM));
}

/**
 * Given a rich player
 * When the pointer moves over the second cell, then the ware is bought on
 * its ticket
 * Then the crown is bought: the pointer chose it
 */
TEST_F(ShopScreenFixture, ThePointerChoosesTheLookItMovesOver) {
  give(A_FORTUNE);
  pointAt(SECOND_CELL_MIDDLE, false);

  buyChosen();

  EXPECT_THAT(wallet().boughtItems, Contains(CROWN_ITEM));
}

/**
 * Given a rich player
 * When the first stall of the glass case is clicked, then BUY on its ticket
 * Then the cheapest hull is bought
 */
TEST_F(ShopScreenFixture, AClickOnAStallThenBuyBuysItsHull) {
  give(A_FORTUNE);

  pointAt(FIRST_STALL_MIDDLE_OF_THE_SHOP, true);
  pointAt(BUY_BUTTON_MIDDLE, true);

  EXPECT_THAT(wallet().boughtItems, Contains(CHEAPEST_HULL_ITEM));
}

/**
 * Given a shop
 * When Escape is pressed and released
 * Then the shop asks to be closed
 */
TEST_F(ShopScreenFixture, EscapeClosesTheShop) {
  press(Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given a rich player, the party hat chosen, in a shop that shows lines
 * whole
 * When Enter is pressed, then BUY on its ticket
 * Then the shopkeeper says the line of the party hat
 */
TEST_F(ShopScreenFixture, ThePurchaseOfALookHasItsLine) {
  give(A_FORTUNE);

  buyChosen();

  EXPECT_NE(writingOf(painting(), PARTY_HAT_LINE_WORD), nullptr);
}

/**
 * Given a player one kopeck short of the party hat
 * When the party hat is bought on its ticket, twice
 * Then the shopkeeper refuses with his first line on a poor player, then
 * with his second
 */
TEST_F(ShopScreenFixture, ThePoorPlayerHearsTheRefusalsInTurn) {
  give(ONE_KOPECK_UNDER_THE_PARTY_HAT);

  buyChosen();
  EXPECT_NE(writingOf(painting(), FIRST_TOO_EXPENSIVE_WORD), nullptr);

  buyChosen();
  EXPECT_NE(writingOf(painting(), SECOND_TOO_EXPENSIVE_WORD), nullptr);
}

/**
 * Given a player who just bought the party hat
 * When Enter is pressed again on it
 * Then the shopkeeper says it is already theirs
 */
TEST_F(ShopScreenFixture, AnOwnedLookHasTheShopkeeperRefuseIt) {
  give(A_FORTUNE);
  buyChosen();

  press(Key::Enter);

  EXPECT_NE(writingOf(painting(), ALREADY_YOURS_WORD), nullptr);
}

/**
 * Given a rich player, the party hat chosen
 * When Enter is pressed once
 * Then the ticket of the party hat shows and nothing is bought yet
 */
TEST_F(ShopScreenFixture, EnterShowsTheTicketBeforeAnyPurchase) {
  give(A_FORTUNE);

  press(Key::Enter);

  EXPECT_TRUE(showsATicket(painting()));
  EXPECT_THAT(wallet().boughtItems, Not(Contains(PARTY_HAT_ITEM)));
}

/**
 * Given a rich player, the ticket of the party hat shown
 * When Escape is pressed
 * Then the ticket goes, nothing is bought, and the shop stays open
 */
TEST_F(ShopScreenFixture, EscapeOnTheTicketCancelsItAndKeepsTheShop) {
  give(A_FORTUNE);
  press(Key::Enter);

  press(Key::Escape);

  EXPECT_FALSE(showsATicket(painting()));
  EXPECT_THAT(wallet().boughtItems, Not(Contains(PARTY_HAT_ITEM)));
  EXPECT_TRUE(navigator().calls().empty());
}

/**
 * Given a rich player, the ticket of the party hat shown
 * When Right chooses CANCEL, then Enter presses it
 * Then the ticket goes and nothing is bought
 */
TEST_F(ShopScreenFixture, EnterOnCancelBuysNothing) {
  give(A_FORTUNE);
  press(Key::Enter);
  press(Key::Right);

  press(Key::Enter);

  EXPECT_FALSE(showsATicket(painting()));
  EXPECT_THAT(wallet().boughtItems, Not(Contains(PARTY_HAT_ITEM)));
}

/**
 * Given a rich player, the ticket of the party hat shown
 * When CANCEL is clicked
 * Then the ticket goes and nothing is bought
 */
TEST_F(ShopScreenFixture, AClickOnCancelBuysNothing) {
  give(A_FORTUNE);
  press(Key::Enter);

  pointAt(CANCEL_BUTTON_MIDDLE, true);

  EXPECT_FALSE(showsATicket(painting()));
  EXPECT_THAT(wallet().boughtItems, Not(Contains(PARTY_HAT_ITEM)));
}

/**
 * Given a rich player, the ticket of the party hat shown
 * When the shop is clicked off the ticket
 * Then the ticket goes and nothing is bought
 */
TEST_F(ShopScreenFixture, AClickOffTheTicketCancelsIt) {
  give(A_FORTUNE);
  press(Key::Enter);

  pointAt(OFF_THE_TICKET, true);

  EXPECT_FALSE(showsATicket(painting()));
  EXPECT_THAT(wallet().boughtItems, Not(Contains(PARTY_HAT_ITEM)));
}

/**
 * Given a rich player who just pressed BUY on the ticket of the party hat,
 * which the master made
 * When a frame shorter than the stamp time passes, then the rest of it
 * Then the paid ticket shows its stamp until its time is up, then goes
 */
TEST_F(ShopScreenFixture, APaidTicketShowsItsStampThenGoes) {
  give(A_FORTUNE);
  press(Key::Enter);
  press(Key::Enter);
  wallet().purchaseAnswer =
      WalletChangeReading{.ask = wallet().boughtItems.size(),
                          .wallet = wallet().balances,
                          .refusal = std::nullopt};
  wait(ONE_FRAME);

  wait(ALMOST_THE_STAMP_TIME);
  EXPECT_TRUE(showsATicket(painting()));

  wait(THE_REST_OF_THE_STAMP_TIME);
  EXPECT_FALSE(showsATicket(painting()));
}

/**
 * Given a player who bought the party hat
 * When Enter is pressed on it again
 * Then no ticket shows: an owned ware is refused at once
 */
TEST_F(ShopScreenFixture, AnOwnedLookShowsNoTicket) {
  give(A_FORTUNE);
  buyChosen();

  press(Key::Enter);

  EXPECT_FALSE(showsATicket(painting()));
}

/**
 * Given a player with no kopeck
 * When the war shark, the first hull of the glass case, is clicked, then BUY
 * on its ticket
 * Then the shopkeeper refuses with the line of the war shark, not with a
 * refusal of an accessory
 */
TEST_F(ShopScreenFixture, AHullTooDearHasItsOwnRefusal) {
  pointAt(FIRST_STALL_MIDDLE_OF_THE_SHOP, true);

  pointAt(BUY_BUTTON_MIDDLE, true);

  EXPECT_NE(writingOf(painting(), WAR_SHARK_TOO_EXPENSIVE_WORD), nullptr);
  EXPECT_EQ(writingOf(painting(), FIRST_TOO_EXPENSIVE_WORD), nullptr);
}

/**
 * Given a rich player who pressed BUY on the ticket of the party hat
 * When Enter is pressed again before the master answered
 * Then the master is asked once: the ticket waits for the answer
 */
TEST_F(ShopScreenFixture, ASecondBuyWaitsForTheAnswerOfTheFirst) {
  give(A_FORTUNE);
  press(Key::Enter);
  press(Key::Enter);

  press(Key::Enter);

  EXPECT_EQ(wallet().boughtItems.size(), 1U);
  EXPECT_TRUE(showsATicket(painting()));
}

/**
 * Given a rich player who pressed BUY on the ticket of the party hat
 * When the master answers that it could not be reached
 * Then the status line says no answer came, and no purchase is heard
 */
TEST_F(ShopScreenFixture, APurchaseTheMasterDidNotMakeSaysWhy) {
  give(A_FORTUNE);
  press(Key::Enter);
  press(Key::Enter);

  wallet().purchaseAnswer =
      WalletChangeReading{.ask = wallet().boughtItems.size(),
                          .wallet = std::nullopt,
                          .refusal = WalletRefusal::MasterUnreachable};
  wait(ONE_FRAME);

  EXPECT_NE(writingOf(painting(), MASTER_SILENT_STATUS.english), nullptr);
  EXPECT_THAT(sounds().played(), Not(Contains(std::string{PURCHASE_SOUND_ID})));
}
