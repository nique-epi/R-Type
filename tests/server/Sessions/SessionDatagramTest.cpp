#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectionAccepted.hpp"
#include "DatagramCodec.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "SessionDatagram.hpp"
#include "SessionDatagramFixture.hpp"
#include "SessionDatagramTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

using testing::LAST_SEQUENCE;
using testing::OTHER_SESSION_TOKEN;
using testing::PLAYER_NICKNAME;
using testing::playerEndpoint;
using testing::SESSION_TOKEN;
using testing::SessionDatagramFixture;
using testing::SOME_NEXT_SEQUENCE;
using testing::SOME_TIME_SINCE_SENT;

/**
 * Given a player the server sent nothing for some time
 * When the header of its next datagram is asked for
 * Then the time since a datagram was sent to it starts again from zero
 */
TEST_F(SessionDatagramFixture, StartsTheTimeSinceSentOverForEachHeader) {
  firstPlayer().sinceSent = SOME_TIME_SINCE_SENT;

  static_cast<void>(nextSessionHeader(firstPlayer()));

  EXPECT_EQ(firstPlayer().sinceSent, engine::Duration::zero());
}

/**
 * Given a player whose next sequence is SOME_NEXT_SEQUENCE
 * When the header of its next datagram is asked for
 * Then the header carries its session token and that sequence, and its next
 * sequence moves on by one
 */
TEST_F(SessionDatagramFixture, GivesTheTokenAndTheNextSequenceOfThePlayer) {
  firstPlayer().nextSequence = SOME_NEXT_SEQUENCE;

  const network::PacketHeader header = nextSessionHeader(firstPlayer());

  EXPECT_EQ(header.sessionToken, SESSION_TOKEN);
  EXPECT_EQ(header.sequence, SOME_NEXT_SEQUENCE);
  EXPECT_EQ(firstPlayer().nextSequence, SOME_NEXT_SEQUENCE + 1);
}

/**
 * Given a player whose next sequence is the last one, 65535
 * When the header of its next datagram is asked for
 * Then the header carries 65535 and the next sequence wraps to 0
 */
TEST_F(SessionDatagramFixture, WrapsTheSequenceAfterTheLastOne) {
  firstPlayer().nextSequence = LAST_SEQUENCE;

  const network::PacketHeader header = nextSessionHeader(firstPlayer());

  EXPECT_EQ(header.sequence, LAST_SEQUENCE);
  EXPECT_EQ(firstPlayer().nextSequence, 0);
}

/**
 * Given two connected players
 * When the header for the address of the second one is asked for
 * Then it carries the token and the first sequence of the second player, and
 * only that player's next sequence moves on
 */
TEST_F(SessionDatagramFixture, FindsThePlayerWhoSendsFromTheRecipient) {
  const ConnectedPlayer* const second = roster().add(
      playerEndpoint(1), std::string{PLAYER_NICKNAME}, OTHER_SESSION_TOKEN);
  ASSERT_NE(second, nullptr);

  const network::PacketHeader header =
      sessionHeaderFor(roster(), playerEndpoint(1));

  EXPECT_EQ(header.sessionToken, OTHER_SESSION_TOKEN);
  EXPECT_EQ(header.sequence, 0);
  EXPECT_EQ(second->nextSequence, 1);
  EXPECT_EQ(firstPlayer().nextSequence, 0);
}

/**
 * Given one connected player
 * When the header for an address no player sends from is asked for
 * Then it carries no session, token 0 and sequence 0, and the player's next
 * sequence does not move
 */
TEST_F(SessionDatagramFixture, GivesNoSessionToAnAddressWithoutPlayer) {
  const network::PacketHeader header =
      sessionHeaderFor(roster(), playerEndpoint(1));

  EXPECT_EQ(header.sessionToken, 0U);
  EXPECT_EQ(header.sequence, 0);
  EXPECT_EQ(firstPlayer().nextSequence, 0);
}

/**
 * Given a connected player
 * When a datagram holding a ConnectionAccepted is built for it
 * Then it goes to the address of the player, starts with the token and the
 * first sequence of its session, and holds that one message
 */
TEST_F(SessionDatagramFixture, AddressesTheMessageToThePlayerUnderItsHeader) {
  const network::ConnectionAccepted accepted{.playerNumber =
                                                 network::FIRST_PLAYER_NUMBER};

  const network::OutgoingDatagram datagram =
      sessionDatagram(firstPlayer(), accepted);

  EXPECT_EQ(datagram.destination, playerEndpoint(0));
  const network::PacketHeader header =
      network::DatagramCodec::decodeHeader(datagram.payload);
  EXPECT_EQ(header.sessionToken, SESSION_TOKEN);
  EXPECT_EQ(header.sequence, 0);
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  ASSERT_EQ(frames.size(), 1U);
  EXPECT_EQ(frames.at(0).type,
            static_cast<std::uint8_t>(network::ConnectionAccepted::TYPE));
  EXPECT_EQ(
      network::decodeMessage<network::ConnectionAccepted>(frames.at(0).payload),
      accepted);
}

/**
 * Given a connected player
 * When a datagram holding a ConnectionAccepted, then one holding an empty
 * GameStarted payload, are built for it
 * Then the second carries the sequence after the first: every kind of message
 * takes its sequence from the same counter
 */
TEST_F(SessionDatagramFixture, NumbersEveryKindOfMessageFromOneCounter) {
  const network::OutgoingDatagram accepted = sessionDatagram(
      firstPlayer(), network::ConnectionAccepted{
                         .playerNumber = network::FIRST_PLAYER_NUMBER});
  const network::OutgoingDatagram started =
      sessionDatagram(firstPlayer(), network::MessageType::GameStarted,
                      std::span<const std::byte>{});

  EXPECT_EQ(network::DatagramCodec::decodeHeader(accepted.payload).sequence, 0);
  EXPECT_EQ(network::DatagramCodec::decodeHeader(started.payload).sequence, 1);
}

}  // namespace rtype::server
