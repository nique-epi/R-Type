#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include "Ambience.hpp"
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "GameRequest.hpp"
#include "GameRules.hpp"
#include "HangarScreenIdConstants.hpp"
#include "LargestServerMapFixture.hpp"
#include "MenuScreenIdConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ServerListPhase.hpp"
#include "ServerListSnapshot.hpp"
#include "ServerMapScreenFixture.hpp"
#include "ServerMapScreenFixtures.hpp"
#include "ServerMapScreenTestConstants.hpp"
#include "ShopScreenIdConstants.hpp"

using rtype::client::Ambience;
using rtype::client::CONNECTION_SCREEN;
using rtype::client::CONNECTION_TO_MAP_SERVER_SCREEN;
using rtype::client::GameRequest;
using rtype::client::HANGAR_SCREEN;
using rtype::client::LEADERBOARD_SCREEN;
using rtype::client::NEW_GAME_SCREEN;
using rtype::client::ServerListPhase;
using rtype::client::ServerListSnapshot;
using rtype::client::SHOP_SCREEN;
using rtype::client::testing::COLUMN_HEADINGS_ON_THE_PANEL;
using rtype::client::testing::HANGAR_HOUSE_ON_THE_MAP;
using rtype::client::testing::HANGAR_HOUSE_SOURCE;
using rtype::client::testing::HANGAR_STAR_HINT_LINE;
using rtype::client::testing::HANGAR_STAR_ON_THE_MAP;
using rtype::client::testing::LargestServerMapFixture;
using rtype::client::testing::listWithLyonHolding;
using rtype::client::testing::listWithNoSeatOfThisVersion;
using rtype::client::testing::LYON_SERVER_ADDRESS;
using rtype::client::testing::LYON_SERVER_ID;
using rtype::client::testing::lyonGames;
using rtype::client::testing::MAP_SHEET;
using rtype::client::testing::NEW_GAME_ROW_ON_THE_PANEL;
using rtype::client::testing::ONE_FRAME;
using rtype::client::testing::ONE_PLANET_TURN_FRAME;
using rtype::client::testing::ONE_SEAT_LEFT;
using rtype::client::testing::PAGES_TO_THE_BOTTOM;
using rtype::client::testing::PARIS_SERVER_ADDRESS;
using rtype::client::testing::PARIS_SERVER_ID;
using rtype::client::testing::parisOnTheWholeMap;
using rtype::client::testing::PLAYING_GAME_ROW_ON_THE_PANEL;
using rtype::client::testing::readyList;
using rtype::client::testing::REFRESH_INTERVAL;
using rtype::client::testing::SCROLLED_BACK_MIDDLE;
using rtype::client::testing::SERVER_GAME_PORT;
using rtype::client::testing::SERVER_SEATS;
using rtype::client::testing::ServerMapScreenFixture;
using rtype::client::testing::SHOP_DETAIL_LINE;
using rtype::client::testing::SHOP_HINT_LINE;
using rtype::client::testing::SHOP_KIOSK_ON_THE_MAP;
using rtype::client::testing::SHOP_KIOSK_SOURCE;
using rtype::client::testing::SHOP_ON_THE_MAP;
using rtype::client::testing::SHORT_OF_THE_REFRESH;
using rtype::client::testing::showsTheMiniMap;
using rtype::client::testing::UNREACHABLE_MASTER;
using rtype::client::testing::WAITING_GAME_NAME;
using rtype::client::testing::WAITING_GAME_NUMBER;
using rtype::client::testing::WAITING_GAME_ROW_ON_THE_PANEL;
using rtype::client::testing::WAITING_GAME_SEATS;
using rtype::network::ANY_GAME_CHOICE;
using rtype::network::defaultGameRules;
using rtype::network::Endpoint;
using rtype::network::GameChoice;
using rtype::network::GameChoiceKind;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Not;

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
 * Given a map
 * When it opens
 * Then the space ambience plays under the theme
 */
TEST_F(ServerMapScreenFixture, OpensWithTheSpaceAmbience) {
  openOn(readyList());

  EXPECT_EQ(audio().ambience(), Ambience::Space);
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
  press(sf::Keyboard::Key::Down);

  press(sf::Keyboard::Key::Enter);

  EXPECT_EQ(choice().lastServer.value_or(Endpoint{}).address,
            std::string{LYON_SERVER_ADDRESS});
  EXPECT_THAT(
      navigator().calls(),
      ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                .screen = CONNECTION_TO_MAP_SERVER_SCREEN}));
}

/**
 * Given the panel of the games of Lyon, its waiting game chosen
 * When Enter is pressed
 * Then the choice asks for that game by its number, with its rules
 */
TEST_F(ServerMapScreenFixture, EnterOnAWaitingGameAsksForThatGame) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Down);

  press(sf::Keyboard::Key::Enter);

  EXPECT_EQ(
      choice().lastRequest,
      (GameRequest{.game = GameChoice{.kind = GameChoiceKind::ExistingGame,
                                      .number = WAITING_GAME_NUMBER},
                   .rules = defaultGameRules()}));
}

/**
 * Given the panel of the games of Lyon open
 * When the left button is pressed on + NEW GAME
 * Then the new game dialog opens over the map, as Enter on it opens it
 */
TEST_F(ServerMapScreenFixture, AClickOnNewGameOpensTheNewGameDialog) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);

  pointAt(NEW_GAME_ROW_ON_THE_PANEL, true);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = NEW_GAME_SCREEN}));
}

/**
 * Given the panel of the games of Lyon open, + NEW GAME chosen
 * When the left button is pressed on its waiting game
 * Then the choice asks for that game, and the connection to Lyon opens
 */
TEST_F(ServerMapScreenFixture, AClickOnAWaitingGameJoinsIt) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);

  pointAt(WAITING_GAME_ROW_ON_THE_PANEL, true);

  EXPECT_EQ(choice().lastRequest.value_or(GameRequest{}).game,
            (GameChoice{.kind = GameChoiceKind::ExistingGame,
                        .number = WAITING_GAME_NUMBER}));
  EXPECT_THAT(
      navigator().calls(),
      ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                .screen = CONNECTION_TO_MAP_SERVER_SCREEN}));
}

/**
 * Given the panel of the games of Lyon open
 * When the left button is pressed on its game in progress
 * Then nothing opens and the choice keeps no attempt, as Enter on it does
 */
TEST_F(ServerMapScreenFixture, AClickOnAGameInProgressJoinsNothing) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);

  pointAt(PLAYING_GAME_ROW_ON_THE_PANEL, true);

  EXPECT_FALSE(choice().lastServer.has_value());
  EXPECT_TRUE(navigator().calls().empty());
}

/**
 * Given the panel of the games of Lyon open, + NEW GAME chosen
 * When the pointer moves over its waiting game, then Enter is pressed
 * Then the waiting game is joined: the pointer chose its row
 */
TEST_F(ServerMapScreenFixture, ThePointerOverARowOfThePanelChoosesIt) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);
  pointAt(WAITING_GAME_ROW_ON_THE_PANEL, false);

  press(sf::Keyboard::Key::Enter);

  EXPECT_EQ(choice().lastRequest.value_or(GameRequest{}).game,
            (GameChoice{.kind = GameChoiceKind::ExistingGame,
                        .number = WAITING_GAME_NUMBER}));
}

/**
 * Given the panel of the games of Lyon open, + NEW GAME chosen
 * When the left button is pressed on the headings of its columns
 * Then nothing opens: only a row acts
 */
TEST_F(ServerMapScreenFixture, AClickOnTheColumnHeadingsActsOnNothing) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);

  pointAt(COLUMN_HEADINGS_ON_THE_PANEL, true);

  EXPECT_TRUE(navigator().calls().empty());
}

/**
 * Given the panel of the games of Lyon, + NEW GAME chosen
 * When Enter is pressed
 * Then the new game dialog opens over the map, and the choice keeps its last
 * attempt: the dialog writes the server and the game together, on CREATE
 */
TEST_F(ServerMapScreenFixture, EnterOnNewGameOpensTheNewGameDialog) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);

  press(sf::Keyboard::Key::Enter);

  EXPECT_FALSE(choice().lastServer.has_value());
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = NEW_GAME_SCREEN}));
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
 * Given the whole map at the largest text size
 * When it shows its top, then Page Down scrolls it to its bottom
 * Then every text off screen at the top shows at the bottom
 */
TEST_F(LargestServerMapFixture, PageDownShowsEveryTextHiddenAtTheTop) {
  openOn(readyList());
  const std::vector<std::string> hiddenAtTheTop = writingsOffScreen(painting());

  for (int page = 0; page < PAGES_TO_THE_BOTTOM; ++page) {
    press(sf::Keyboard::Key::PageDown);
  }

  ASSERT_THAT(hiddenAtTheTop, Not(IsEmpty()));
  const std::vector<std::string> hiddenAtTheBottom =
      writingsOffScreen(painting());
  for (const std::string& text : hiddenAtTheTop) {
    EXPECT_THAT(hiddenAtTheBottom, Not(Contains(text))) << text;
  }
}

/**
 * Given the panel of the games of Lyon at the largest text size, its waiting
 * game chosen
 * When it is painted
 * Then no line runs past a side of the screen
 */
TEST_F(LargestServerMapFixture, NoLineOfThePanelRunsPastASide) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);

  press(sf::Keyboard::Key::Down);

  EXPECT_THAT(writingsOutSideways(painting()), IsEmpty());
}

/**
 * Given the panel of the games of Lyon at the largest text size
 * When it is painted
 * Then the pilots of the waiting game stand under its name, on a line of
 * their own
 */
TEST_F(LargestServerMapFixture, AGameWritesItsPilotsUnderItsName) {
  openOn(readyList());
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);

  press(sf::Keyboard::Key::Enter);

  const RecordedWriting* name = writingOf(painting(), WAITING_GAME_NAME);
  const RecordedWriting* seats = writingOf(painting(), WAITING_GAME_SEATS);
  ASSERT_NE(name, nullptr);
  ASSERT_NE(seats, nullptr);
  const sf::IntRect nameBox = writtenBoxOf(painting(), *name);
  const sf::IntRect seatsBox = writtenBoxOf(painting(), *seats);
  EXPECT_GE(seatsBox.position.y, nameBox.position.y + nameBox.size.y);
}

/**
 * Given the whole map at the largest text size, scrolled to its bottom
 * When the left button clicks the middle of BACK where it shows
 * Then the map closes
 */
TEST_F(LargestServerMapFixture, AClickOnTheScrolledBackClosesTheMap) {
  openOn(readyList());
  for (int page = 0; page < PAGES_TO_THE_BOTTOM; ++page) {
    press(sf::Keyboard::Key::PageDown);
  }

  pointAt(SCROLLED_BACK_MIDDLE, true);

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
 * Given Lyon at 24 ms with every seat taken, and Paris at 88 ms with seats
 * When Q is pressed and released
 * Then Paris is joined: the lowest ping among the servers with a free seat
 */
TEST_F(ServerMapScreenFixture, QuickPlaySkipsAFullServerForTheNextByPing) {
  openOn(listWithLyonHolding(SERVER_SEATS));

  press(sf::Keyboard::Key::Q);

  EXPECT_EQ(choice().lastServer.value_or(Endpoint{}).address,
            std::string{PARIS_SERVER_ADDRESS});
}

/**
 * Given Lyon at 24 ms with one seat left of its sixteen
 * When Q is pressed and released
 * Then Lyon is joined: a server is full only once every seat is taken
 */
TEST_F(ServerMapScreenFixture, QuickPlayJoinsAServerWithOneSeatLeft) {
  openOn(listWithLyonHolding(ONE_SEAT_LEFT));

  press(sf::Keyboard::Key::Q);

  EXPECT_EQ(choice().lastServer.value_or(Endpoint{}).address,
            std::string{LYON_SERVER_ADDRESS});
}

/**
 * Given Lyon and Paris full, and an old station of another version with seats
 * When Q is pressed and released
 * Then no server is joined and no screen is pushed: a connection there would
 * be refused
 */
TEST_F(ServerMapScreenFixture, QuickPlayJoinsNothingWhenNoServerHasASeat) {
  openOn(listWithNoSeatOfThisVersion());

  press(sf::Keyboard::Key::Q);

  EXPECT_FALSE(choice().lastServer.has_value());
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given Lyon at 24 ms and Paris at 88 ms, both with seats, and an old station
 * When Q joins Lyon
 * Then Paris is kept as the server to try if Lyon answers that it is full,
 * and the old station is not
 */
TEST_F(ServerMapScreenFixture, QuickPlayKeepsTheOtherFreeServersToTryNext) {
  openOn(readyList());

  press(sf::Keyboard::Key::Q);

  EXPECT_THAT(choice().nextServers,
              ElementsAre(Endpoint{.address = std::string{PARIS_SERVER_ADDRESS},
                                   .port = SERVER_GAME_PORT}));
}

/**
 * Given Q joined Lyon and kept Paris, then the player came back to the map
 * When a waiting game of the panel of Lyon is joined
 * Then no server is kept: a full server named by the player is not left for
 * another
 */
TEST_F(ServerMapScreenFixture, JoiningAGameOfThePanelKeepsNoOtherServer) {
  openOn(readyList());
  press(sf::Keyboard::Key::Q);
  games().show(lyonGames());
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Enter);
  press(sf::Keyboard::Key::Down);

  press(sf::Keyboard::Key::Enter);

  EXPECT_THAT(choice().nextServers, IsEmpty());
}

/**
 * Given the whole map
 * When Q joins Lyon
 * Then the choice asks for any game, played by the default rules: the only
 * games a server places such a player in
 */
TEST_F(ServerMapScreenFixture, QuickPlayAsksForAnyGame) {
  openOn(readyList());

  press(sf::Keyboard::Key::Q);

  EXPECT_EQ(choice().lastRequest, (GameRequest{.game = ANY_GAME_CHOICE,
                                               .rules = defaultGameRules()}));
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

/**
 * Given the map
 * When it is painted
 * Then the hangar star is the golden house of the sheet of the map, centered
 * where the star stands
 */
TEST_F(ServerMapScreenFixture, TheHangarStarIsDrawnAsAHouse) {
  openOn(readyList());

  EXPECT_TRUE(
      std::ranges::any_of(painting().images, [](const RecordedImage& image) {
        return image.assetId == MAP_SHEET &&
               image.source == HANGAR_HOUSE_SOURCE &&
               image.topLeft == HANGAR_HOUSE_ON_THE_MAP;
      }));
}

/**
 * Given a master that did not answer, so no server stands on the map
 * When the kiosk of the shop is clicked
 * Then the shop opens over the map: it needs no server
 */
TEST_F(ServerMapScreenFixture, ClickingTheKioskOpensTheShop) {
  openOn(ServerListSnapshot{.phase = ServerListPhase::MasterUnreachable,
                            .servers = {}});

  pointAt(SHOP_ON_THE_MAP, true);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SHOP_SCREEN}));
}

/**
 * Given the map
 * When S is pressed and released
 * Then the shop opens over it
 */
TEST_F(ServerMapScreenFixture, SOpensTheShop) {
  openOn(readyList());

  press(sf::Keyboard::Key::S);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SHOP_SCREEN}));
}

/**
 * Given the map
 * When the pointer moves over the kiosk of the shop
 * Then its bubble says what the shop sells and how to open it
 */
TEST_F(ServerMapScreenFixture, TheKioskShowsItsBubbleUnderThePointer) {
  openOn(readyList());

  pointAt(SHOP_ON_THE_MAP, false);

  EXPECT_NE(writingOf(painting(), SHOP_DETAIL_LINE), nullptr);
  EXPECT_NE(writingOf(painting(), SHOP_HINT_LINE), nullptr);
}

/**
 * Given the map, the pointer over the hangar star, whose bubble shows
 * When the pointer moves over the kiosk of the shop
 * Then the bubble of the hangar goes: one place shows its bubble at a time
 */
TEST_F(ServerMapScreenFixture, TheKioskTakesTheBubbleOfTheHangar) {
  openOn(readyList());
  pointAt(HANGAR_STAR_ON_THE_MAP, false);

  pointAt(SHOP_ON_THE_MAP, false);

  EXPECT_EQ(writingOf(painting(), HANGAR_STAR_HINT_LINE), nullptr);
  EXPECT_NE(writingOf(painting(), SHOP_HINT_LINE), nullptr);
}

/**
 * Given the map, the pointer over the kiosk of the shop
 * When an arrow chooses a server
 * Then the bubble of the shop goes: only the chosen server shows one
 */
TEST_F(ServerMapScreenFixture, AnArrowHidesTheBubbleOfTheShop) {
  openOn(readyList());
  pointAt(SHOP_ON_THE_MAP, false);

  press(sf::Keyboard::Key::Right);

  EXPECT_EQ(writingOf(painting(), SHOP_HINT_LINE), nullptr);
}

/**
 * Given the map
 * When it is painted
 * Then the shop is the kiosk of the sheet of the map, centered where the
 * shop stands
 */
TEST_F(ServerMapScreenFixture, TheShopIsDrawnAsAKiosk) {
  openOn(readyList());

  EXPECT_TRUE(
      std::ranges::any_of(painting().images, [](const RecordedImage& image) {
        return image.assetId == MAP_SHEET &&
               image.source == SHOP_KIOSK_SOURCE &&
               image.topLeft == SHOP_KIOSK_ON_THE_MAP;
      }));
}

/**
 * Given the map, for a player who asked for less motion
 * When the time of a frame of the turn of a planet passes
 * Then every planet still shows the first frame of its row
 */
TEST_F(ServerMapScreenFixture, WithLessMotionThePlanetsStandStill) {
  openOn(readyList());

  update(ONE_PLANET_TURN_FRAME);

  ASSERT_TRUE(std::ranges::any_of(
      painting().images,
      [](const RecordedImage& image) { return image.assetId == MAP_SHEET; }));
  for (const RecordedImage& image : painting().images) {
    if (image.assetId == MAP_SHEET) {
      EXPECT_EQ(image.source.position.x, 0);
    }
  }
}
