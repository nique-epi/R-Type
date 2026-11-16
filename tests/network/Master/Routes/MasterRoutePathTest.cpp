#include <gtest/gtest.h>
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "MasterRoutePathTestConstants.hpp"
#include "RoomRound.hpp"

using rtype::network::BUY_OWN_LOOK_ROUTE;
using rtype::network::gamePathOf;
using rtype::network::KEEP_OWN_RESULT_ROUTE;
using rtype::network::LIST_SERVER_GAMES_ROUTE;
using rtype::network::lookPathOf;
using rtype::network::PLACE_OWN_BETS_ROUTE;
using rtype::network::roomPathOf;
using rtype::network::RoomRound;
using rtype::network::roundPathOf;
using rtype::network::serverPathOf;
using rtype::network::TAKE_OWN_SEAT_ROUTE;

/**
 * Given the route that lists the games of a server
 * When its path is made for the server 7
 * Then it is `/api/servers/7/games`
 */
TEST(MasterRoutePath, ThePathOfTheGamesNamesTheServer) {
  EXPECT_EQ(serverPathOf(LIST_SERVER_GAMES_ROUTE, SEVENTH_SERVER_ID),
            SEVENTH_SERVER_GAMES_PATH);
}

/**
 * Given the route that keeps the result of a game of the server of the key
 * When its path is made for a game id
 * Then it is `/api/servers/me/results/` followed by that id
 */
TEST(MasterRoutePath, ThePathOfAResultNamesTheGame) {
  EXPECT_EQ(gamePathOf(KEEP_OWN_RESULT_ROUTE, SOME_GAME_ID),
            SOME_GAME_RESULT_PATH);
}

/**
 * Given the route that seats the account of the token in a room
 * When its path is made for the room 2
 * Then it is `/api/casino/rooms/2/seats/me`
 */
TEST(MasterRoutePath, ThePathOfASeatNamesTheRoom) {
  EXPECT_EQ(roomPathOf(TAKE_OWN_SEAT_ROUTE, SECOND_ROOM),
            SECOND_ROOM_SEAT_PATH);
}

/**
 * Given the route that places the bets of a round
 * When its path is made for the room 2 and the round 815
 * Then it names both
 */
TEST(MasterRoutePath, ThePathOfTheBetsNamesTheRoomAndTheRound) {
  EXPECT_EQ(roundPathOf(PLACE_OWN_BETS_ROUTE,
                        RoomRound{.room = SECOND_ROOM, .round = SOME_ROUND}),
            SOME_ROUND_BETS_PATH);
}

/**
 * Given the route that buys a look for the account of the token
 * When its path is made for the crown
 * Then it is `/api/users/me/looks/crown`
 */
TEST(MasterRoutePath, ThePathOfAPurchaseNamesTheLook) {
  EXPECT_EQ(lookPathOf(BUY_OWN_LOOK_ROUTE, CROWN_LOOK), CROWN_PURCHASE_PATH);
}
