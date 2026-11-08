#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string>
#include <vector>
#include "Endpoint.hpp"
#include "HangarScreenIdConstants.hpp"
#include "MenuScreenIdConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ServerListPhase.hpp"
#include "ServerListSnapshot.hpp"
#include "ServerMapScreenFixture.hpp"
#include "ServerMapScreenFixtures.hpp"
#include "ServerMapScreenTestConstants.hpp"

using rtype::client::CONNECTION_SCREEN;
using rtype::client::CONNECTION_TO_CHOSEN_SERVER_SCREEN;
using rtype::client::HANGAR_SCREEN;
using rtype::client::LEADERBOARD_SCREEN;
using rtype::client::MASTER_SCREEN;
using rtype::client::ServerListPhase;
using rtype::client::ServerListSnapshot;
using rtype::client::testing::HANGAR_STAR_HINT_LINE;
using rtype::client::testing::HANGAR_STAR_ON_THE_MAP;
using rtype::client::testing::LYON_SERVER_ADDRESS;
using rtype::client::testing::LYON_SERVER_ID;
using rtype::client::testing::lyonGames;
using rtype::client::testing::MAP_MASTER_TEXT;
using rtype::client::testing::ONE_FRAME;
using rtype::client::testing::PARIS_SERVER_ID;
using rtype::client::testing::parisOnTheWholeMap;
using rtype::client::testing::readyList;
using rtype::client::testing::REFRESH_INTERVAL;
using rtype::client::testing::ServerMapScreenFixture;
using rtype::client::testing::SHORT_OF_THE_REFRESH;
using rtype::client::testing::showsTheMiniMap;
using rtype::client::testing::UNREACHABLE_MASTER;
using rtype::network::Endpoint;
using ::testing::ElementsAre;

/**
 * Given a server map being built
 * When it opens
 * Then it reads the list of servers once
 */
TEST_F(ServerMapScreenFixture, ReadsTheListAsItOpens) {
  openOn(readyList());

  EXPECT_EQ(servers().refreshCount(), 1U);
}

/**
 * Given a map that opened 14 s ago
 * When one more second passes, 15 s in all
 * Then it reads the list again on its own
 */
TEST_F(ServerMapScreenFixture, ReadsTheListAgainEveryFifteenSeconds) {
  openOn(readyList());
  update(SHORT_OF_THE_REFRESH);
  ASSERT_EQ(servers().refreshCount(), 1U);

  update(REFRESH_INTERVAL - SHORT_OF_THE_REFRESH);

  EXPECT_EQ(servers().refreshCount(), 2U);
}

/**
 * Given a map of three servers
 * When R is pressed
 * Then the list is read again at once
 */
TEST_F(ServerMapScreenFixture, TheRefreshKeyReadsTheListAgain) {
  openOn(readyList());

  press(sf::Keyboard::Key::R);

  EXPECT_EQ(servers().refreshCount(), 2U);
}

/**
 * Given the whole map, without its minimap
 * When Enter is pressed
 * Then the map zooms on the chosen server, its minimap shown
 */
TEST_F(ServerMapScreenFixture, EnterZoomsOnTheChosenServer) {
  openOn(readyList());
  ASSERT_FALSE(showsTheMiniMap(painting()));

  press(sf::Keyboard::Key::Enter);

  EXPECT_TRUE(showsTheMiniMap(painting()));
}

/**
 * Given the map zoomed on Lyon, the lowest ping, chosen first
 * When Enter is pressed again
 * Then the games of Lyon are asked for
 */
TEST_F(ServerMapScreenFixture, EnterOnTheZoomAsksForTheGamesOfTheServer) {
  openOn(readyList());
  press(sf::Keyboard::Key::Enter);

  press(sf::Keyboard::Key::Enter);

  EXPECT_THAT(games().asked(), ElementsAre(LYON_SERVER_ID));
}

/**
 * Given the whole map, Lyon chosen first
 * When the left button is pressed on the star of Paris
 * Then the games of Paris are asked for at once, not those of Lyon
 */
TEST_F(ServerMapScreenFixture, AClickOnAStarOpensTheGamesOfItsServer) {
  openOn(readyList());

  pointAt(parisOnTheWholeMap(), true);

  EXPECT_THAT(games().asked(), ElementsAre(PARIS_SERVER_ID));
}

/**
 * Given the whole map, Lyon chosen first
 * When the pointer moves over the star of Paris, then Enter zooms and Enter
 * opens the games
 * Then the games of Paris are asked for: the pointer chose it
 */
TEST_F(ServerMapScreenFixture, ThePointerOverAStarChoosesItsServer) {
  openOn(readyList());
  pointAt(parisOnTheWholeMap(), false);

  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);

  EXPECT_THAT(games().asked(), ElementsAre(PARIS_SERVER_ID));
}

/**
 * Given Enter held down on the whole map, never released
 * When its press repeats
 * Then the map zooms once and asks for no games
 */
TEST_F(ServerMapScreenFixture, AHeldEnterActsOnce) {
  openOn(readyList());

  hold(sf::Keyboard::Key::Enter);
  hold(sf::Keyboard::Key::Enter);

  EXPECT_TRUE(games().asked().empty());
}

/**
 * Given the panel of the games of Lyon, its waiting game chosen
 * When Enter is pressed
 * Then Lyon goes in the choice as the server of the last attempt, and the
 * connection to it opens
 */
TEST_F(ServerMapScreenFixture, EnterOnAWaitingGameJoinsItsServer) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);

  press(sf::Keyboard::Key::Enter);

  EXPECT_EQ(choice().lastServer.value_or(Endpoint{}).address,
            std::string{LYON_SERVER_ADDRESS});
  EXPECT_THAT(
      navigator().calls(),
      ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                .screen = CONNECTION_TO_CHOSEN_SERVER_SCREEN}));
}

/**
 * Given the panel of the games of Lyon, the game in progress chosen
 * When Enter is pressed
 * Then nothing is joined
 */
TEST_F(ServerMapScreenFixture, EnterOnAGameInProgressJoinsNothing) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Down);

  press(sf::Keyboard::Key::Enter);

  EXPECT_TRUE(navigator().calls().empty());
}

/**
 * Given the map zoomed on a server
 * When Escape is pressed and released
 * Then the whole map comes back, and the map stays open
 */
TEST_F(ServerMapScreenFixture, EscapeOnTheZoomGoesBackToTheWholeMap) {
  openOn(readyList());
  press(sf::Keyboard::Key::Enter);

  press(sf::Keyboard::Key::Escape);

  EXPECT_FALSE(showsTheMiniMap(painting()));
  EXPECT_TRUE(navigator().calls().empty());
}

/**
 * Given the whole map
 * When Escape is pressed and released
 * Then the map closes
 */
TEST_F(ServerMapScreenFixture, EscapeOnTheWholeMapClosesIt) {
  openOn(readyList());

  press(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given Lyon of this version at 24 ms, Paris at 88 ms, and an old station
 * When Q is pressed and released
 * Then Lyon is joined: the server of this version with the lowest ping
 */
TEST_F(ServerMapScreenFixture, QuickPlayJoinsTheNearestServerOfThisVersion) {
  openOn(readyList());

  press(sf::Keyboard::Key::Q);

  EXPECT_EQ(choice().lastServer.value_or(Endpoint{}).address,
            std::string{LYON_SERVER_ADDRESS});
}

/**
 * Given the whole map, its choice holding no server yet
 * When Q joins Lyon
 * Then the server the form opened by ADDRESS remembers stays as it was
 */
TEST_F(ServerMapScreenFixture, JoiningLeavesTheServerOfTheForm) {
  openOn(readyList());

  press(sf::Keyboard::Key::Q);

  EXPECT_EQ(choice().server, Endpoint{});
}

/**
 * Given the whole map, Lyon chosen first
 * When Tab is pressed
 * Then Paris, the next by ping, is chosen: Enter twice asks for its games
 */
TEST_F(ServerMapScreenFixture, TabChoosesTheNextServerByPing) {
  openOn(readyList());

  press(sf::Keyboard::Key::Tab);
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);

  EXPECT_THAT(games().asked(), ElementsAre(PARIS_SERVER_ID));
}

/**
 * Given the map
 * When L is pressed and released
 * Then the leaderboard opens over it
 */
TEST_F(ServerMapScreenFixture, RankingOpensTheLeaderboard) {
  openOn(readyList());

  press(sf::Keyboard::Key::L);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = LEADERBOARD_SCREEN}));
}

/**
 * Given a master that did not answer
 * When D is pressed and released
 * Then the form of a direct connection opens: it stays a way out
 */
TEST_F(ServerMapScreenFixture, AddressOpensTheFormEvenWithoutAMaster) {
  openOn(ServerListSnapshot{.phase = ServerListPhase::MasterUnreachable,
                            .servers = {}});

  press(sf::Keyboard::Key::D);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CONNECTION_SCREEN}));
}

/**
 * Given the map, its master not answering
 * When M is pressed and released
 * Then the form of the master opens over it
 */
TEST_F(ServerMapScreenFixture, MasterOpensTheMasterForm) {
  openOn(ServerListSnapshot{.phase = ServerListPhase::MasterUnreachable,
                            .servers = {}});

  press(sf::Keyboard::Key::M);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = MASTER_SCREEN}));
}

/**
 * Given the map of a master at 10.0.0.2:8080
 * When the map is painted
 * Then it writes that master, without http://
 */
TEST_F(ServerMapScreenFixture, TheMapNamesTheMasterItReads) {
  openOn(readyList());

  EXPECT_NE(writingOf(painting(), MAP_MASTER_TEXT), nullptr);
}

/**
 * Given a master that did not answer
 * When the map is painted
 * Then it says the master is unreachable
 */
TEST_F(ServerMapScreenFixture, SaysWhenTheMasterIsUnreachable) {
  openOn(ServerListSnapshot{.phase = ServerListPhase::MasterUnreachable,
                            .servers = {}});

  EXPECT_NE(writingOf(painting(), UNREACHABLE_MASTER), nullptr);
}

/**
 * Given a master that did not answer, so no server is shown
 * When Enter is pressed
 * Then nothing is zoomed nor asked
 */
TEST_F(ServerMapScreenFixture, EnterWithoutAServerDoesNothing) {
  openOn(ServerListSnapshot{.phase = ServerListPhase::MasterUnreachable,
                            .servers = {}});

  press(sf::Keyboard::Key::Enter);
  update(ONE_FRAME);

  EXPECT_TRUE(games().asked().empty());
  EXPECT_TRUE(navigator().calls().empty());
}

/**
 * Given a master that did not answer, so no server stands on the map
 * When the hangar star is clicked
 * Then the hangar opens over the map: it needs no server
 */
TEST_F(ServerMapScreenFixture, ClickingTheHangarStarOpensTheHangar) {
  openOn(ServerListSnapshot{.phase = ServerListPhase::MasterUnreachable,
                            .servers = {}});

  pointAt(HANGAR_STAR_ON_THE_MAP, true);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = HANGAR_SCREEN}));
}

/**
 * Given the map
 * When G is pressed and released
 * Then the hangar opens over it
 */
TEST_F(ServerMapScreenFixture, GOpensTheHangar) {
  openOn(readyList());

  press(sf::Keyboard::Key::G);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = HANGAR_SCREEN}));
}

/**
 * Given the map
 * When the pointer moves over the hangar star
 * Then its bubble tells how to open it
 */
TEST_F(ServerMapScreenFixture, TheHangarStarShowsItsBubbleUnderThePointer) {
  openOn(readyList());

  pointAt(HANGAR_STAR_ON_THE_MAP, false);

  EXPECT_NE(writingOf(painting(), HANGAR_STAR_HINT_LINE), nullptr);
}

/**
 * Given the map, the pointer over the hangar star
 * When an arrow chooses a server
 * Then the bubble of the hangar goes: only the chosen server shows one
 */
TEST_F(ServerMapScreenFixture, AnArrowHidesTheBubbleOfTheHangar) {
  openOn(readyList());
  pointAt(HANGAR_STAR_ON_THE_MAP, false);

  press(sf::Keyboard::Key::Right);

  EXPECT_EQ(writingOf(painting(), HANGAR_STAR_HINT_LINE), nullptr);
}
