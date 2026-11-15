#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <vector>
#include "ClothBand.hpp"
#include "ClothBets.hpp"
#include "ClothBetsFixtures.hpp"
#include "ClothBetsTestConstants.hpp"
#include "ClothPlace.hpp"
#include "RouletteBets.hpp"

using rtype::client::betAt;
using rtype::client::ClothBand;
using rtype::client::ClothPlace;
using rtype::client::placeOf;
using rtype::client::testing::AMONG_ONE_TWO_FOUR_FIVE;
using rtype::client::testing::AMONG_THIRTY_TWO_TO_THIRTY_SIX;
using rtype::client::testing::BETWEEN_ONE_AND_FOUR;
using rtype::client::testing::BETWEEN_TWO_AND_THREE;
using rtype::client::testing::COLUMNS;
using rtype::client::testing::CORNERS;
using rtype::client::testing::DOZENS;
using rtype::client::testing::ON_NINETEEN_TO_THIRTY_SIX;
using rtype::client::testing::ON_SEVENTEEN;
using rtype::client::testing::ON_THE_RED_DIAMOND;
using rtype::client::testing::ON_THE_THIRD_DOZEN;
using rtype::client::testing::ON_THE_TOP_COLUMN;
using rtype::client::testing::ON_THE_ZERO;
using rtype::client::testing::ONE;
using rtype::client::testing::ONE_BET;
using rtype::client::testing::reachableClothPlaces;
using rtype::client::testing::serverTakes;
using rtype::client::testing::SEVENTEEN;
using rtype::client::testing::SIX_LINES;
using rtype::client::testing::SPLITS;
using rtype::client::testing::SPLITS_ACROSS;
using rtype::client::testing::STRAIGHTS;
using rtype::client::testing::STREETS;
using rtype::client::testing::THIRTY_FOUR;
using rtype::client::testing::THIRTY_ONE;
using rtype::client::testing::THIRTY_TWO;
using rtype::client::testing::THREE;
using rtype::client::testing::TWENTY_FIVE;
using rtype::client::testing::TWO;
using rtype::client::testing::UNDER_THE_LAST_SIX_LINE;
using rtype::client::testing::UNDER_THE_LAST_STREET;
using rtype::network::RouletteBet;
using rtype::network::RouletteBetKind;

/**
 * Given the cursor on a number
 * When the bet it makes is read
 * Then it is a straight on that number
 */
TEST(ClothBets, ANumberHoldsAStraight) {
  EXPECT_EQ(betAt(ON_SEVENTEEN), (RouletteBet{.kind = RouletteBetKind::Straight,
                                              .firstNumber = SEVENTEEN}));
}

/**
 * Given the cursor on the zero, come from the bottom row
 * When the bet it makes is read
 * Then it is a straight on the zero
 */
TEST(ClothBets, TheZeroHoldsAStraightOnZero) {
  EXPECT_EQ(betAt(ON_THE_ZERO),
            (RouletteBet{.kind = RouletteBetKind::Straight, .firstNumber = 0}));
}

/**
 * Given the cursor on the line between 2 and 3, one street
 * When the bet it makes is read
 * Then it is a split from 2
 */
TEST(ClothBets, TheLineInsideAStreetHoldsASplit) {
  EXPECT_EQ(betAt(BETWEEN_TWO_AND_THREE),
            (RouletteBet{.kind = RouletteBetKind::Split, .firstNumber = TWO}));
}

/**
 * Given the cursor on the line between 1 and 4, two streets
 * When the bet it makes is read
 * Then it is a split across from 1
 */
TEST(ClothBets, TheLineBetweenStreetsHoldsASplitAcross) {
  EXPECT_EQ(
      betAt(BETWEEN_ONE_AND_FOUR),
      (RouletteBet{.kind = RouletteBetKind::SplitAcross, .firstNumber = ONE}));
}

/**
 * Given the cursor where 1, 2, 4 and 5 meet, then where 32, 33, 35 and 36
 * meet
 * When the bets they make are read
 * Then they are corners from 1 and from 32
 */
TEST(ClothBets, WhereFourNumbersMeetHoldsACorner) {
  EXPECT_EQ(betAt(AMONG_ONE_TWO_FOUR_FIVE),
            (RouletteBet{.kind = RouletteBetKind::Corner, .firstNumber = ONE}));
  EXPECT_EQ(betAt(AMONG_THIRTY_TWO_TO_THIRTY_SIX),
            (RouletteBet{.kind = RouletteBetKind::Corner,
                         .firstNumber = THIRTY_TWO}));
}

/**
 * Given the cursor on the edge under the last street
 * When the bet it makes is read
 * Then it is the street from 34
 */
TEST(ClothBets, TheEdgeUnderAStreetHoldsTheStreet) {
  EXPECT_EQ(betAt(UNDER_THE_LAST_STREET),
            (RouletteBet{.kind = RouletteBetKind::Street,
                         .firstNumber = THIRTY_FOUR}));
}

/**
 * Given the cursor where the edge meets the line between the last two
 * streets
 * When the bet it makes is read
 * Then it is the six line from 31
 */
TEST(ClothBets, TheEdgeBetweenStreetsHoldsASixLine) {
  EXPECT_EQ(betAt(UNDER_THE_LAST_SIX_LINE),
            (RouletteBet{.kind = RouletteBetKind::SixLine,
                         .firstNumber = THIRTY_ONE}));
}

/**
 * Given the cursor on the column at the end of the top row, 3, 6... 36
 * When the bet it makes is read
 * Then it is the column from 3
 */
TEST(ClothBets, TheTopColumnStartsAtThree) {
  EXPECT_EQ(
      betAt(ON_THE_TOP_COLUMN),
      (RouletteBet{.kind = RouletteBetKind::Column, .firstNumber = THREE}));
}

/**
 * Given the cursor on the third dozen
 * When the bet it makes is read
 * Then it is the dozen from 25
 */
TEST(ClothBets, TheThirdDozenStartsAtTwentyFive) {
  EXPECT_EQ(betAt(ON_THE_THIRD_DOZEN),
            (RouletteBet{.kind = RouletteBetKind::Dozen,
                         .firstNumber = TWENTY_FIVE}));
}

/**
 * Given the cursor on the red diamond, then on 19-36
 * When the bets they make are read
 * Then they are red and high, on no number
 */
TEST(ClothBets, TheEvenChancesNameNoNumber) {
  EXPECT_EQ(betAt(ON_THE_RED_DIAMOND),
            (RouletteBet{.kind = RouletteBetKind::Red, .firstNumber = 0}));
  EXPECT_EQ(betAt(ON_NINETEEN_TO_THIRTY_SIX),
            (RouletteBet{.kind = RouletteBetKind::High, .firstNumber = 0}));
}

/**
 * Given every place the arrows reach
 * When the bet each makes is read
 * Then the server takes each one
 */
TEST(ClothBets, TheServerTakesEveryBetOfTheCloth) {
  for (const ClothPlace& place : reachableClothPlaces()) {
    EXPECT_TRUE(serverTakes(betAt(place)))
        << "across " << place.across << ", down " << place.down;
  }
}

/**
 * Given every place the arrows reach, the zero from any row
 * When the different bets they make are counted by kind
 * Then the cloth holds every bet of a single zero table
 */
TEST(ClothBets, TheArrowsReachEveryBetOfTheCloth) {
  std::vector<RouletteBet> bets;
  for (const ClothPlace& place : reachableClothPlaces()) {
    if (std::ranges::find(bets, betAt(place)) == bets.end()) {
      bets.push_back(betAt(place));
    }
  }
  const auto count = [&bets](RouletteBetKind kind) {
    return static_cast<std::size_t>(
        std::ranges::count(bets, kind, &RouletteBet::kind));
  };

  EXPECT_EQ(count(RouletteBetKind::Straight), STRAIGHTS);
  EXPECT_EQ(count(RouletteBetKind::Split), SPLITS);
  EXPECT_EQ(count(RouletteBetKind::SplitAcross), SPLITS_ACROSS);
  EXPECT_EQ(count(RouletteBetKind::Street), STREETS);
  EXPECT_EQ(count(RouletteBetKind::Corner), CORNERS);
  EXPECT_EQ(count(RouletteBetKind::SixLine), SIX_LINES);
  EXPECT_EQ(count(RouletteBetKind::Dozen), DOZENS);
  EXPECT_EQ(count(RouletteBetKind::Column), COLUMNS);
  for (const RouletteBetKind kind :
       {RouletteBetKind::Red, RouletteBetKind::Black, RouletteBetKind::Even,
        RouletteBetKind::Odd, RouletteBetKind::Low, RouletteBetKind::High}) {
    EXPECT_EQ(count(kind), ONE_BET);
  }
}

/**
 * Given every place the arrows reach but the zero, whose row the cursor
 * keeps
 * When the place of the bet each makes is read
 * Then it is that place, where the chips of the bet are piled
 */
TEST(ClothBets, EveryBetLiesBackOnItsPlace) {
  for (const ClothPlace& place : reachableClothPlaces()) {
    if (place.across != 0 || place.band != ClothBand::Numbers) {
      EXPECT_EQ(placeOf(betAt(place)), place)
          << "across " << place.across << ", down " << place.down;
    }
  }
}
