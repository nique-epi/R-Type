#include <gtest/gtest.h>
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "MasterRoutePathTestConstants.hpp"

using rtype::network::LIST_SERVER_GAMES_ROUTE;
using rtype::network::serverPathOf;

/**
 * Given the route that lists the games of a server
 * When its path is made for the server 7
 * Then it is `/api/servers/7/games`
 */
TEST(MasterRoutePath, ThePathOfTheGamesNamesTheServer) {
  EXPECT_EQ(serverPathOf(LIST_SERVER_GAMES_ROUTE, SEVENTH_SERVER_ID),
            SEVENTH_SERVER_GAMES_PATH);
}
