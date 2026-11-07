#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ChatChannel.hpp"
#include "ChatLine.hpp"
#include "ContentLoading.hpp"
#include "ContentPackCache.hpp"
#include "GameLoadedReminder.hpp"
#include "MessageDispatcher.hpp"
#include "PlayerChatLine.hpp"
#include "RecordingDatagramLink.hpp"
#include "RecordingWaitingRoom.hpp"
#include "ServerSession.hpp"
#include "ServerSessionParts.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionEntityTypes.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A session and the chat that follows it, wired as PlayerConnection
 * wires them, over a recording link and a recording waiting room. */
class ChatChannelFixture : public ::testing::Test {
 protected:
  /** @brief Connects, then has the server accept the attempt, its acceptance
   * carrying SESSION_TOKEN. */
  void connectAndGetAccepted();

  /** @brief Runs one frame of the network stage. */
  void update(engine::Duration elapsed = engine::Duration::zero());

  /** @brief The server relays @p relayed, in a datagram of the next sequence,
   * then a frame goes by. */
  void relay(const network::PlayerChatLine& relayed);

  [[nodiscard]] ChatChannel& chat();
  [[nodiscard]] RecordingWaitingRoom& room();

  /** @returns The chat lines sent to the server, copies included. */
  [[nodiscard]] std::vector<network::ChatLine> sentLines() const;

 private:
  RecordingDatagramLink link_;
  network::MessageDispatcher dispatcher_;
  GameLoadedReminder gameLoaded_;
  RecordingWaitingRoom room_;
  ChatChannel chat_{dispatcher_, room_};
  SessionEntityTypes entityTypes_{testLocalContent()};
  ContentLoading content_{ContentPackCache{unusedCacheFolder()}};
  ServerSession session_{ServerSessionParts{.link = link_,
                                            .dispatcher = dispatcher_,
                                            .gameLoaded = gameLoaded_,
                                            .entityTypes = entityTypes_,
                                            .content = content_,
                                            .observers = {gameLoaded_, chat_}}};
  std::uint16_t nextSequence_ = FIRST_SEQUENCE;
};

}  // namespace rtype::client::testing
