#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include <vector>
#include "ByteText.hpp"
#include "ConnectionPhase.hpp"
#include "ContentAnnouncement.hpp"
#include "ContentPackDownloadFixtures.hpp"
#include "ContentPackDownloadTestConstants.hpp"
#include "ContentPackTestFiles.hpp"
#include "ContentPackTestFilesConstants.hpp"
#include "EntityType.hpp"
#include "NumberedEntityType.hpp"
#include "ServerSessionContentFixture.hpp"
#include "ServerSessionContentTestConstants.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "SessionEntityTypesTestConstants.hpp"

namespace rtype::client::testing {

using network::textOf;

/**
 * Given a client connecting to a server
 * When the server accepts it announcing the client's own content
 * Then the session opens at once, numbering the client's own types
 */
TEST_F(ServerSessionContentFixture, ItsOwnContentOpensTheSessionAtOnce) {
  connectAndGetAcceptedWith(localAnnouncement());

  EXPECT_EQ(session().status().phase, ConnectionPhase::Accepted);
  const game::NumberedEntityType* const ship =
      entityTypes().entryNumbered(SECOND_TYPE_NUMBER);
  ASSERT_NE(ship, nullptr);
  EXPECT_EQ(ship->name, DRAWN_SHIP_NAME);
  EXPECT_EQ(ship->side, game::EntityType::Player);
}

/**
 * Given a session of the client's own content, of two types
 * When an entity of its third number is looked up
 * Then it names no type, so the entity is skipped
 */
TEST_F(ServerSessionContentFixture, ANumberPastTheContentNamesNoType) {
  connectAndGetAcceptedWith(localAnnouncement());

  EXPECT_EQ(entityTypes().entryNumbered(THIRD_TYPE_NUMBER), nullptr);
}

/**
 * Given a server that plays another content and holds its pack back
 * When it accepts the client
 * Then the session holds its seat while the pack loads, and leaves nothing
 */
TEST_F(ServerSessionContentFixture, HoldsTheSessionWhileThePackLoads) {
  const network::ContentAnnouncement content =
      serveTheLancerPackAs(textOf(lancerPack()));
  supervisor().holdTheAnswers();

  connectAndGetAcceptedWith(content);

  EXPECT_EQ(session().status().phase, ConnectionPhase::LoadingContent);
  EXPECT_EQ(session().status().contentSize, content.size);
  EXPECT_TRUE(disconnectionTokens().empty());
  supervisor().releaseTheAnswers();
}

/**
 * Given a server that plays the pack of a Bydo this client has no sheet for
 * When the client is accepted and the pack downloaded
 * Then the session opens, numbering the types of the pack
 */
TEST_F(ServerSessionContentFixture, PlaysWithThePackOnceItLoaded) {
  connectAndGetAcceptedWith(serveTheLancerPackAs(textOf(lancerPack())));

  updateWhileLoading();

  EXPECT_EQ(session().status().phase, ConnectionPhase::Accepted);
  const game::NumberedEntityType* const lancer =
      entityTypes().entryNumbered(FIRST_TYPE_NUMBER);
  ASSERT_NE(lancer, nullptr);
  EXPECT_EQ(lancer->name, LANCER_TYPE);
  EXPECT_NE(entityTypes().appearances().named(LANCER_TYPE), nullptr);
}

/**
 * Given a client that downloaded the pack of a server, then left it
 * When it is accepted again by that server
 * Then the session opens at once from the kept pack, with no request
 */
TEST_F(ServerSessionContentFixture, OpensAtOnceWithAKeptPack) {
  const network::ContentAnnouncement content =
      serveTheLancerPackAs(textOf(lancerPack()));
  connectAndGetAcceptedWith(content);
  updateWhileLoading();
  session().cancel();

  connectAndGetAcceptedWith(content);

  EXPECT_EQ(session().status().phase, ConnectionPhase::Accepted);
  EXPECT_EQ(supervisor().requests(), ONE_REQUEST);
}

/**
 * Given a session that played the pack of a server
 * When the session is left
 * Then the client numbers its own types again
 */
TEST_F(ServerSessionContentFixture, ComesBackToItsOwnContentOnceLeft) {
  connectAndGetAcceptedWith(serveTheLancerPackAs(textOf(lancerPack())));
  updateWhileLoading();

  session().cancel();

  const game::NumberedEntityType* const first =
      entityTypes().entryNumbered(FIRST_TYPE_NUMBER);
  ASSERT_NE(first, nullptr);
  EXPECT_EQ(first->name, DRAWN_ENEMY_NAME);
}

/**
 * Given a server whose supervisor serves no pack
 * When it accepts the client announcing another content
 * Then the attempt ends at once as a failed download, and a Disconnection
 * carrying the token of the acceptance goes out, so the seat is free again
 */
TEST_F(ServerSessionContentFixture, LeavesAtOnceWhenNoPackIsServed) {
  connectAndGetAcceptedWith(otherAnnouncement(ANY_PACK_SIZE, NO_PACK_PORT));

  EXPECT_EQ(session().status().phase, ConnectionPhase::ContentDownloadFailed);
  EXPECT_EQ(disconnectionTokens(), std::vector<std::uint64_t>{SESSION_TOKEN});
}

/**
 * Given a server that refuses the pack it announced
 * When the client is accepted and its download fails
 * Then the session is left as a failed download, its Disconnection sent
 */
TEST_F(ServerSessionContentFixture, LeavesTheSessionWhenTheDownloadFails) {
  const network::ContentAnnouncement content =
      serveTheLancerPackAs(textOf(lancerPack()));
  supervisor().answerEveryRequestWith(PACK_NOT_FOUND_STATUS);
  connectAndGetAcceptedWith(content);

  updateWhileLoading();

  EXPECT_EQ(session().status().phase, ConnectionPhase::ContentDownloadFailed);
  EXPECT_EQ(disconnectionTokens(), std::vector<std::uint64_t>{SESSION_TOKEN});
}

/**
 * Given a server that sends another pack than the one it announced, a pack
 * that reads well and has the announced size
 * When the client is accepted and downloads it
 * Then it is refused for its fingerprint, never played with, and the session
 * is left
 */
TEST_F(ServerSessionContentFixture, RefusesAPackThatIsNotTheAnnouncedOne) {
  connectAndGetAcceptedWith(
      serveTheLancerPackAs(textOf(packNumbered(ANOTHER_PACK_SEED))));

  updateWhileLoading();

  EXPECT_EQ(session().status().phase, ConnectionPhase::ContentRefused);
  EXPECT_EQ(disconnectionTokens(), std::vector<std::uint64_t>{SESSION_TOKEN});
  const game::NumberedEntityType* const first =
      entityTypes().entryNumbered(FIRST_TYPE_NUMBER);
  ASSERT_NE(first, nullptr);
  EXPECT_EQ(first->name, DRAWN_ENEMY_NAME);
}

/**
 * Given a session of the client's own content
 * When the server repeats its acceptance announcing another content, as for
 * a request that crossed the first answer
 * Then the session keeps its content
 */
TEST_F(ServerSessionContentFixture, ARepeatedAcceptanceChangesNothing) {
  connectAndGetAcceptedWith(localAnnouncement());

  getAcceptedAgainWith(otherAnnouncement(ANY_PACK_SIZE, NO_PACK_PORT));

  EXPECT_EQ(session().status().phase, ConnectionPhase::Accepted);
  EXPECT_TRUE(disconnectionTokens().empty());
}

}  // namespace rtype::client::testing
