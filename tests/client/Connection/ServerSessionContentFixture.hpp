#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "ContentAnnouncement.hpp"
#include "ContentLoading.hpp"
#include "ContentPackCache.hpp"
#include "Endpoint.hpp"
#include "FakePackServer.hpp"
#include "GameLoadedReminder.hpp"
#include "MessageDispatcher.hpp"
#include "RecordingDatagramLink.hpp"
#include "ServerSession.hpp"
#include "ServerSessionParts.hpp"
#include "SessionEntityTypes.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "TemporaryFolder.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A session and the content it plays with, wired as PlayerConnection
 * wires them, over a recording link, with a cache in a folder of the test and,
 * when a test starts one, a fake supervisor serving the pack of the lancer on
 * the loopback. */
class ServerSessionContentFixture : public ::testing::Test {
 protected:
  /** @returns The server of the session, on the loopback: the address the
   * pack is downloaded from. */
  [[nodiscard]] static network::Endpoint loopbackServer();

  /** @brief Connects, then has the server accept the attempt announcing
   * @p content, its acceptance carrying SESSION_TOKEN, and runs the frame
   * that reads it. */
  void connectAndGetAcceptedWith(const network::ContentAnnouncement& content);

  /** @brief Delivers another acceptance announcing @p content, in a newer
   * datagram, and runs the frame that reads it. */
  void getAcceptedAgainWith(const network::ContentAnnouncement& content);

  /** @brief Starts a fake supervisor answering the pack of the lancer with
   * @p body. @returns What its acceptance announces: the lancer pack, its
   * size and the port of the fake. */
  network::ContentAnnouncement serveTheLancerPackAs(const std::string& body);

  /** @returns The fake supervisor, once serveTheLancerPackAs() started it. */
  [[nodiscard]] FakePackServer& supervisor();

  /** @brief Runs frames of LOADING_FRAME while the content loads, at most
   * LOADING_WAIT. */
  void updateWhileLoading();

  void update(engine::Duration elapsed = engine::Duration::zero());

  [[nodiscard]] const SessionEntityTypes& entityTypes() const;

  [[nodiscard]] ServerSession& session();

  /** @returns The session token of each Disconnection sent so far. */
  [[nodiscard]] std::vector<std::uint64_t> disconnectionTokens() const;

 private:
  TemporaryFolder cacheFolder_;
  std::unique_ptr<FakePackServer> supervisor_;
  RecordingDatagramLink link_;
  network::MessageDispatcher dispatcher_;
  GameLoadedReminder gameLoaded_;
  SessionEntityTypes entityTypes_{testLocalContent()};
  ContentLoading content_{ContentPackCache{cacheFolder_.path()}};
  ServerSession session_{ServerSessionParts{.link = link_,
                                            .dispatcher = dispatcher_,
                                            .gameLoaded = gameLoaded_,
                                            .entityTypes = entityTypes_,
                                            .content = content_,
                                            .observers = {gameLoaded_}}};
};

}  // namespace rtype::client::testing
