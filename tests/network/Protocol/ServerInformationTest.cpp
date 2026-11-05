#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <vector>
#include "ByteSequence.hpp"
#include "DatagramCodec.hpp"
#include "MasterMessageTextTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationFixtures.hpp"
#include "ServerInformationRequestTestConstants.hpp"
#include "ServerInformationTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given the answer of a server of two games, five players and three free seats
 * When it is encoded
 * Then the payload holds its fields in the order of the protocol page
 */
TEST(ServerInformation, WritesItsFieldsInTheOrderOfTheProtocolPage) {
  const auto expected =
      byteSequence({0x01, 0x02, 0x03, 0x04, 0x0A, 0x0B, 0x0C, 0x0D, 0x03,
                    0x05, '0',  '.',  '1',  '.',  '0',  0x04, 'L',  'y',
                    'o',  'n',  0x00, 0x05, 0x00, 0x02, 0x00, 0x03});

  EXPECT_EQ(encodeMessage(someServerInformation()), expected);
}

/**
 * Given an encoded answer
 * When it is decoded
 * Then it is the answer that was encoded
 */
TEST(ServerInformation, ComesBackTheSame) {
  const ServerInformation answer = someServerInformation();

  EXPECT_EQ(decodeMessage<ServerInformation>(encodeMessage(answer)), answer);
}

/**
 * Given an answer with the longest name and game version
 * When it is put in a datagram
 * Then the datagram holds the 106 bytes of the protocol page, no more than a
 * request: the server never sends more than it received
 */
TEST(ServerInformation, TheLargestAnswerIsNoLargerThanARequest) {
  ServerInformation answer = someServerInformation();
  answer.gameVersion = std::string{NAME_OF_32_BYTES};
  answer.name = std::string{NAME_OF_32_BYTES};

  const std::vector<std::byte> datagram =
      DatagramCodec::encode(PacketHeader{}, answer);

  EXPECT_EQ(datagram.size(), LARGEST_ANSWER_DATAGRAM_ON_THE_PROTOCOL_PAGE);
  EXPECT_LE(datagram.size(), REQUEST_DATAGRAM_ON_THE_PROTOCOL_PAGE);
}

/**
 * Given names of 0, 32 and 33 bytes
 * When each answer is encoded
 * Then only the name of 32 bytes is accepted
 */
TEST(ServerInformation, BoundsTheSizeOfTheName) {
  ServerInformation answer = someServerInformation();
  answer.name = "";
  EXPECT_THROW((void)encodeMessage(answer), InvalidServerTextException);
  answer.name = std::string{NAME_OF_32_BYTES};
  EXPECT_NO_THROW((void)encodeMessage(answer));
  answer.name = std::string{NAME_OF_33_BYTES};
  EXPECT_THROW((void)encodeMessage(answer), InvalidServerTextException);
}

/**
 * Given game versions of 0, 32 and 33 bytes
 * When each answer is encoded
 * Then only the version of 32 bytes is accepted
 */
TEST(ServerInformation, BoundsTheSizeOfTheGameVersion) {
  ServerInformation answer = someServerInformation();
  answer.gameVersion = "";
  EXPECT_THROW((void)encodeMessage(answer), InvalidServerTextException);
  answer.gameVersion = std::string{NAME_OF_32_BYTES};
  EXPECT_NO_THROW((void)encodeMessage(answer));
  answer.gameVersion = std::string{NAME_OF_33_BYTES};
  EXPECT_THROW((void)encodeMessage(answer), InvalidServerTextException);
}

/**
 * Given a received answer whose name holds a line feed
 * When it is decoded
 * Then it is refused, so the name never starts a line of a log
 */
TEST(ServerInformation, RefusesANameWithAControlCharacter) {
  const auto payload = byteSequence({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                     0x00, 0x03, 0x01, '1',  0x02, 'A',  '\n',
                                     0x00, 0x00, 0x00, 0x01, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<ServerInformation>(payload),
               InvalidServerTextException);
}

/**
 * Given two games, so eight seats, and eight then nine players
 * When each answer is encoded
 * Then eight players are accepted and nine are refused
 */
TEST(ServerInformation, SeatsNoMorePlayersThanTheGamesHold) {
  ServerInformation answer = someServerInformation();
  answer.freeSeatCount = 0;
  answer.playerCount = SEATS_OF_TWO_GAMES;
  EXPECT_NO_THROW((void)encodeMessage(answer));
  answer.playerCount = SEATS_OF_TWO_GAMES + 1;
  EXPECT_THROW((void)encodeMessage(answer), InvalidFieldValueException);
}

/**
 * Given two games, five players, and three then four free seats
 * When each answer is encoded
 * Then three free seats are accepted and four are refused: the players and the
 * free seats together never pass the seats of the games
 */
TEST(ServerInformation, CountsNoMoreFreeSeatsThanAreLeft) {
  ServerInformation answer = someServerInformation();
  answer.freeSeatCount = THREE_FREE_SEATS;
  EXPECT_NO_THROW((void)encodeMessage(answer));
  answer.freeSeatCount = THREE_FREE_SEATS + 1;
  EXPECT_THROW((void)encodeMessage(answer), InvalidFieldValueException);
}

/**
 * Given an answer cut before its last byte, and one with a byte after it
 * When each is decoded
 * Then the first is refused as short, the second for its trailing byte
 */
TEST(ServerInformation, RefusesAPayloadOfTheWrongSize) {
  std::vector<std::byte> payload = encodeMessage(someServerInformation());
  std::vector<std::byte> cut = payload;
  cut.pop_back();
  payload.push_back(std::byte{0});

  EXPECT_THROW((void)decodeMessage<ServerInformation>(cut),
               BufferUnderflowException);
  EXPECT_THROW((void)decodeMessage<ServerInformation>(payload),
               TrailingBytesException);
}

}  // namespace rtype::network::testing
