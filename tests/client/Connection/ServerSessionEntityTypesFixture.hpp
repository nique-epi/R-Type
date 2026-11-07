#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "EntityTypeEntry.hpp"
#include "GameLoadedReminder.hpp"
#include "MessageDispatcher.hpp"
#include "RecordingDatagramLink.hpp"
#include "ServerSession.hpp"
#include "ServerSessionParts.hpp"
#include "SessionEntityTypes.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "ShipGun.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A session and the table of entity types it keeps, wired as
 * PlayerConnection wires them, over a recording link, the client drawing what
 * isDrawnInTheTests() names: the tests have the server accept with a table and
 * read the status, the table and what went out. */
class ServerSessionEntityTypesFixture : public ::testing::Test {
 protected:
  /** @brief Connects, then has the server accept the attempt with @p types,
   * its acceptance carrying SESSION_TOKEN, and runs the frame that reads
   * it. */
  void connectAndGetAcceptedWith(std::vector<network::EntityTypeEntry> types,
                                 network::ShipGun gun = network::ShipGun{});

  /** @brief Delivers another acceptance carrying @p types, in a datagram of
   * sequence REPEATED_ACCEPTANCE_SEQUENCE and SESSION_TOKEN, and runs the
   * frame that reads it. */
  void getAcceptedAgainWith(std::vector<network::EntityTypeEntry> types);

  /** @brief Runs one frame of the network stage. */
  void update(engine::Duration elapsed = engine::Duration::zero());

  [[nodiscard]] const SessionEntityTypes& entityTypes() const;

  [[nodiscard]] ServerSession& session();

  /** @returns The session token of each Disconnection sent so far, in
   * order. */
  [[nodiscard]] std::vector<std::uint64_t> disconnectionTokens() const;

 private:
  RecordingDatagramLink link_;
  network::MessageDispatcher dispatcher_;
  GameLoadedReminder gameLoaded_;
  SessionEntityTypes entityTypes_{isDrawnInTheTests};
  ServerSession session_{ServerSessionParts{.link = link_,
                                            .dispatcher = dispatcher_,
                                            .gameLoaded = gameLoaded_,
                                            .entityTypes = entityTypes_,
                                            .observers = {gameLoaded_}}};
};

}  // namespace rtype::client::testing
