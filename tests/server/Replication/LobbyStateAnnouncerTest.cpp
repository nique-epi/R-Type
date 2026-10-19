#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "LobbyState.hpp"
#include "LobbyStateAnnouncer.hpp"
#include "LobbyStateAnnouncerTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {
namespace {

using testing::LONGEST_LOBBY_STATE_GAP;
using testing::onlyMessageIn;
using testing::OTHER_SESSION_TOKEN;
using testing::PLAYER_NICKNAME;
using testing::playerEndpoint;
using testing::SESSION_TOKEN;

/** @returns The line of the room for the player at @p index, under the test
 * nickname. */
network::LobbyPlayer lobbyPlayer(std::size_t index, bool ready) {
  return network::LobbyPlayer{.playerNumber = static_cast<std::uint8_t>(
                                  network::FIRST_PLAYER_NUMBER + index),
                              .ready = ready,
                              .nickname = std::string{PLAYER_NICKNAME}};
}

/** @returns How many ticks fit in the longest gap the protocol allows. */
std::size_t ticksInTheLongestGap() {
  return static_cast<std::size_t>(LONGEST_LOBBY_STATE_GAP /
                                  engine::SIMULATION_TICK_DURATION);
}

/** @brief An announcer over a roster, whose datagrams are recorded instead of
 * sent. */
class LobbyStateAnnouncerTest : public ::testing::Test {
 protected:
  /** @brief Connects the player number @p index + 1, under @p sessionToken. */
  ConnectedPlayer& join(std::size_t index,
                        std::uint64_t sessionToken = SESSION_TOKEN) {
    ConnectedPlayer* const player = roster_.add(
        playerEndpoint(index), std::string{PLAYER_NICKNAME}, sessionToken);
    return *player;
  }

  /** @brief Lets @p count ticks of the waiting room pass. */
  void runWaitingTicks(std::size_t count) {
    for (std::size_t tick = 0; tick < count; ++tick) {
      announcer_.onWaitingTick();
    }
  }

  /** @returns The datagrams sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> sentTo(
      std::size_t index) const {
    return testing::sentTo(sent_, playerEndpoint(index));
  }

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const {
    return sent_;
  }

 private:
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  LobbyStateAnnouncer announcer_{roster_,
                                 [this](network::OutgoingDatagram datagram) {
                                   sent_.push_back(std::move(datagram));
                                 }};
};

/**
 * Given two connected players, the second one ready
 * When the first tick of the waiting room passes
 * Then each player is sent one room listing both, in order of number, with
 * their ready state and their nickname
 */
TEST_F(LobbyStateAnnouncerTest, SendsTheRoomAtTheFirstTick) {
  join(0);
  join(1).ready = true;

  runWaitingTicks(1);

  const network::LobbyState expected{
      .players = {lobbyPlayer(0, false), lobbyPlayer(1, true)}};
  const std::vector<network::OutgoingDatagram> toFirst = sentTo(0);
  const std::vector<network::OutgoingDatagram> toSecond = sentTo(1);
  ASSERT_EQ(toFirst.size(), 1U);
  ASSERT_EQ(toSecond.size(), 1U);
  EXPECT_EQ(onlyMessageIn<network::LobbyState>(toFirst.front()), expected);
  EXPECT_EQ(onlyMessageIn<network::LobbyState>(toSecond.front()), expected);
}

/**
 * Given one player, sent the room
 * When the player becomes ready and the next tick passes
 * Then the player is sent the room again, showing it ready
 */
TEST_F(LobbyStateAnnouncerTest, SendsAChangeOfReadinessAtOnce) {
  ConnectedPlayer& player = join(0);
  runWaitingTicks(1);

  player.ready = true;
  runWaitingTicks(1);

  const std::vector<network::OutgoingDatagram> toPlayer = sentTo(0);
  ASSERT_EQ(toPlayer.size(), 2U);
  EXPECT_EQ(onlyMessageIn<network::LobbyState>(toPlayer.back()),
            (network::LobbyState{.players = {lobbyPlayer(0, true)}}));
}

/**
 * Given one player, sent the room
 * When a second player joins and the next tick passes
 * Then both players are sent the room listing the two of them
 */
TEST_F(LobbyStateAnnouncerTest, SendsANewPlayerAtOnce) {
  join(0);
  runWaitingTicks(1);

  join(1);
  runWaitingTicks(1);

  const network::LobbyState expected{
      .players = {lobbyPlayer(0, false), lobbyPlayer(1, false)}};
  const std::vector<network::OutgoingDatagram> toFirst = sentTo(0);
  const std::vector<network::OutgoingDatagram> toSecond = sentTo(1);
  ASSERT_EQ(toFirst.size(), 2U);
  ASSERT_EQ(toSecond.size(), 1U);
  EXPECT_EQ(onlyMessageIn<network::LobbyState>(toFirst.back()), expected);
  EXPECT_EQ(onlyMessageIn<network::LobbyState>(toSecond.back()), expected);
}

/**
 * Given one player, sent the room
 * When the next tick passes without change
 * Then nothing more is sent: an unchanged room is not sent every tick
 */
TEST_F(LobbyStateAnnouncerTest, DoesNotSendAnUnchangedRoomAtTheNextTick) {
  join(0);
  runWaitingTicks(1);

  runWaitingTicks(1);

  EXPECT_EQ(sentTo(0).size(), 1U);
}

/**
 * Given one player, sent the room
 * When as many ticks pass without change as fit in the longest gap the
 * protocol allows between two LobbyState, 500 ms
 * Then the player was sent the room again within them
 */
TEST_F(LobbyStateAnnouncerTest, RepeatsAnUnchangedRoomWithinTheLongestGap) {
  join(0);
  runWaitingTicks(1);

  runWaitingTicks(ticksInTheLongestGap());

  EXPECT_GE(sentTo(0).size(), 2U);
}

/**
 * Given two players with their own session tokens, the second one already
 * sent one datagram of its session
 * When the room is sent
 * Then the datagram to each player carries its token and its next sequence
 */
TEST_F(LobbyStateAnnouncerTest, CarriesTheTokenAndTheNextSequenceOfEachPlayer) {
  join(0, SESSION_TOKEN);
  join(1, OTHER_SESSION_TOKEN).nextSequence = 1;

  runWaitingTicks(1);

  const std::vector<network::OutgoingDatagram> toFirst = sentTo(0);
  const std::vector<network::OutgoingDatagram> toSecond = sentTo(1);
  ASSERT_EQ(toFirst.size(), 1U);
  ASSERT_EQ(toSecond.size(), 1U);
  const network::PacketHeader firstHeader =
      network::DatagramCodec::decodeHeader(toFirst.front().payload);
  const network::PacketHeader secondHeader =
      network::DatagramCodec::decodeHeader(toSecond.front().payload);
  EXPECT_EQ(firstHeader.sessionToken, SESSION_TOKEN);
  EXPECT_EQ(firstHeader.sequence, 0);
  EXPECT_EQ(secondHeader.sessionToken, OTHER_SESSION_TOKEN);
  EXPECT_EQ(secondHeader.sequence, 1);
}

/**
 * Given a waiting room with no player
 * When as many ticks pass as fit in the longest gap
 * Then nothing is sent
 */
TEST_F(LobbyStateAnnouncerTest, SendsNothingWithoutPlayers) {
  runWaitingTicks(ticksInTheLongestGap());

  EXPECT_TRUE(sent().empty());
}

}  // namespace
}  // namespace rtype::server
