#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "DatagramCodec.hpp"
#include "GameEnded.hpp"
#include "GameStarted.hpp"
#include "LobbyState.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolTestConstants.hpp"
#include "ReadyState.hpp"

namespace rtype::network {
namespace {

using testing::ACCENTED_LETTER;
using testing::byteSequence;
using testing::LARGEST_LOBBY_DATAGRAM_SIZE;
using testing::LAST_PLAYER_NUMBER;
using testing::ONE_BYTE_NICKNAME;
using testing::SIXTEEN_BYTE_NICKNAME;
using testing::SOME_SCORE_OF_DISTINCT_BYTES;

LobbyPlayer lobbyPlayer(std::uint8_t playerNumber) {
  return LobbyPlayer{.playerNumber = playerNumber,
                     .ready = false,
                     .nickname = std::string{SIXTEEN_BYTE_NICKNAME}};
}

LobbyState fullLobby() {
  return LobbyState{.players = {lobbyPlayer(1), lobbyPlayer(2), lobbyPlayer(3),
                                lobbyPlayer(4)}};
}

/**
 * Given a player who is ready
 * When the ReadyState is encoded
 * Then the payload is the byte 01
 */
TEST(ReadyState, EncodesReadyAsOne) {
  const auto expected = byteSequence({0x01});

  EXPECT_EQ(encodeMessage(ReadyState{.ready = true}), expected);
}

/**
 * Given a player who is not ready
 * When the ReadyState is encoded then decoded
 * Then the decoded state is not ready
 */
TEST(ReadyState, DecodesWhatItEncoded) {
  const ReadyState notReady{.ready = false};

  EXPECT_EQ(decodeMessage<ReadyState>(encodeMessage(notReady)), notReady);
}

/**
 * Given the payload 02
 * When it is decoded as a ReadyState
 * Then an InvalidBoolException is raised
 */
TEST(ReadyState, RefusesAByteOtherThanZeroOrOne) {
  const auto bytes = byteSequence({0x02});

  EXPECT_THROW((void)decodeMessage<ReadyState>(bytes), InvalidBoolException);
}

/**
 * Given an empty payload
 * When it is decoded as a ReadyState
 * Then a BufferUnderflowException is raised
 */
TEST(ReadyState, RefusesAnEmptyPayload) {
  const std::vector<std::byte> empty;

  EXPECT_THROW((void)decodeMessage<ReadyState>(empty),
               BufferUnderflowException);
}

/**
 * Given a ready byte followed by one more byte
 * When it is decoded as a ReadyState
 * Then a TrailingBytesException is raised
 */
TEST(ReadyState, RefusesBytesAfterTheReadyByte) {
  const auto bytes = byteSequence({0x01, 0x00});

  EXPECT_THROW((void)decodeMessage<ReadyState>(bytes), TrailingBytesException);
}

/**
 * Given player 1, ready, named A, and player 2, not ready, named with an
 * accented letter
 * When the LobbyState is encoded
 * Then the payload is the count 2, then each player: number, ready, length
 * and nickname
 */
TEST(LobbyState, EncodesThePlayersInOrder) {
  const LobbyState lobby{
      .players = {{.playerNumber = 1,
                   .ready = true,
                   .nickname = std::string{ONE_BYTE_NICKNAME}},
                  {.playerNumber = 2,
                   .ready = false,
                   .nickname = std::string{ACCENTED_LETTER}}}};
  const auto expected = byteSequence(
      {0x02, 0x01, 0x01, 0x01, 0x41, 0x02, 0x00, 0x02, 0xC3, 0xA9});

  EXPECT_EQ(encodeMessage(lobby), expected);
}

/**
 * Given a waiting room without players
 * When the LobbyState is encoded
 * Then the payload is the count 0 alone
 */
TEST(LobbyState, EncodesAnEmptyRoomAsACountOfZero) {
  const auto expected = byteSequence({0x00});

  EXPECT_EQ(encodeMessage(LobbyState{}), expected);
}

/**
 * Given four players with sixteen-byte nicknames
 * When the LobbyState is encoded then decoded
 * Then the decoded state equals the original
 */
TEST(LobbyState, DecodesWhatItEncoded) {
  EXPECT_EQ(decodeMessage<LobbyState>(encodeMessage(fullLobby())), fullLobby());
}

/**
 * Given the bytes of a room with one ready player named A
 * When they are decoded then encoded again
 * Then the same bytes come back
 */
TEST(LobbyState, EncodesAgainTheBytesItDecoded) {
  const auto bytes = byteSequence({0x01, 0x03, 0x01, 0x01, 0x41});

  EXPECT_EQ(encodeMessage(decodeMessage<LobbyState>(bytes)), bytes);
}

/**
 * Given four players with sixteen-byte nicknames, the largest room
 * When its datagram is encoded
 * Then it holds 102 bytes, well within a datagram
 */
TEST(LobbyState, LargestRoomDatagramHoldsOneHundredAndTwoBytes) {
  EXPECT_EQ(DatagramCodec::encode(PacketHeader{}, fullLobby()).size(),
            LARGEST_LOBBY_DATAGRAM_SIZE);
}

/**
 * Given a payload announcing five players and holding nothing more
 * When it is decoded
 * Then a TooManyPlayersException is raised before any player is read
 */
TEST(LobbyState, RefusesToReadFivePlayersBeforeReadingThem) {
  const auto bytes = byteSequence({0x05});

  EXPECT_THROW((void)decodeMessage<LobbyState>(bytes), TooManyPlayersException);
}

/**
 * Given a room of five players
 * When it is encoded
 * Then a TooManyPlayersException is raised
 */
TEST(LobbyState, RefusesToWriteFivePlayers) {
  LobbyState lobby = fullLobby();
  lobby.players.push_back(lobbyPlayer(LAST_PLAYER_NUMBER));

  EXPECT_THROW((void)encodeMessage(lobby), TooManyPlayersException);
}

/**
 * Given a payload whose player has the number 0
 * When it is decoded
 * Then an InvalidFieldValueException is raised
 */
TEST(LobbyState, RefusesToReadAPlayerNumberOutsideOneToFour) {
  const auto bytes = byteSequence({0x01, 0x00, 0x01, 0x01, 0x41});

  EXPECT_THROW((void)decodeMessage<LobbyState>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a payload listing player 1 twice
 * When it is decoded
 * Then a DuplicatePlayerNumberException is raised
 */
TEST(LobbyState, RefusesToReadTheSamePlayerTwice) {
  const auto bytes =
      byteSequence({0x02, 0x01, 0x01, 0x01, 0x41, 0x01, 0x00, 0x01, 0x41});

  EXPECT_THROW((void)decodeMessage<LobbyState>(bytes),
               DuplicatePlayerNumberException);
}

/**
 * Given a room listing player 2 twice
 * When it is encoded
 * Then a DuplicatePlayerNumberException is raised
 */
TEST(LobbyState, RefusesToWriteTheSamePlayerTwice) {
  const LobbyState lobby{.players = {lobbyPlayer(2), lobbyPlayer(2)}};

  EXPECT_THROW((void)encodeMessage(lobby), DuplicatePlayerNumberException);
}

/**
 * Given a payload whose player has an empty nickname
 * When it is decoded
 * Then an InvalidNicknameException is raised
 */
TEST(LobbyState, RefusesToReadAnEmptyNickname) {
  const auto bytes = byteSequence({0x01, 0x01, 0x01, 0x00});

  EXPECT_THROW((void)decodeMessage<LobbyState>(bytes),
               InvalidNicknameException);
}

/**
 * Given a payload whose ready byte is 2
 * When it is decoded
 * Then an InvalidBoolException is raised
 */
TEST(LobbyState, RefusesToReadAReadyByteOtherThanZeroOrOne) {
  const auto bytes = byteSequence({0x01, 0x01, 0x02, 0x01, 0x41});

  EXPECT_THROW((void)decodeMessage<LobbyState>(bytes), InvalidBoolException);
}

/**
 * Given a room whose third player has an empty nickname
 * When writing it is refused
 * Then nothing was written, not even the count and the first two players
 */
TEST(LobbyState, RefusedRoomWritesNothing) {
  LobbyState lobby = fullLobby();
  lobby.players[2].nickname.clear();
  ByteWriter writer;

  EXPECT_THROW(encodePayload(writer, lobby), InvalidNicknameException);

  EXPECT_TRUE(writer.bytes().empty());
}

/**
 * Given a payload announcing one player and holding only its number
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(LobbyState, RefusesAPlayerCutShort) {
  const auto bytes = byteSequence({0x01, 0x01});

  EXPECT_THROW((void)decodeMessage<LobbyState>(bytes),
               BufferUnderflowException);
}

/**
 * Given a whole room followed by one more byte
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(LobbyState, RefusesBytesAfterTheLastPlayer) {
  const auto bytes = byteSequence({0x01, 0x01, 0x01, 0x01, 0x41, 0x00});

  EXPECT_THROW((void)decodeMessage<LobbyState>(bytes), TrailingBytesException);
}

/**
 * Given the start of a game
 * When it is encoded
 * Then the payload is empty
 */
TEST(GameStarted, EncodesAnEmptyPayload) {
  EXPECT_TRUE(encodeMessage(GameStarted{}).empty());
}

/**
 * Given an empty payload
 * When it is decoded as GameStarted
 * Then GameStarted comes back
 */
TEST(GameStarted, DecodesAnEmptyPayload) {
  const std::vector<std::byte> empty;

  EXPECT_EQ(decodeMessage<GameStarted>(empty), GameStarted{});
}

/**
 * Given a payload of one byte
 * When it is decoded as GameStarted
 * Then a TrailingBytesException is raised
 */
TEST(GameStarted, RefusesAnyByteInThePayload) {
  const auto bytes = byteSequence({0x00});

  EXPECT_THROW((void)decodeMessage<GameStarted>(bytes), TrailingBytesException);
}

/**
 * Given the end of a game where player 1 scored 0x01020304
 * When it is encoded
 * Then the payload is the count 1, the number 1, then the score on four bytes
 */
TEST(GameEnded, EncodesTheScores) {
  const GameEnded ended{
      .players = {{.playerNumber = 1, .score = SOME_SCORE_OF_DISTINCT_BYTES}}};
  const auto expected = byteSequence({0x01, 0x01, 0x01, 0x02, 0x03, 0x04});

  EXPECT_EQ(encodeMessage(ended), expected);
}

/**
 * Given the end of a game of four players
 * When it is encoded then decoded
 * Then the decoded end equals the original
 */
TEST(GameEnded, DecodesWhatItEncoded) {
  const GameEnded ended{
      .players = {{.playerNumber = 4, .score = SOME_SCORE_OF_DISTINCT_BYTES},
                  {.playerNumber = 3, .score = 0},
                  {.playerNumber = 2, .score = 1},
                  {.playerNumber = 1, .score = SOME_SCORE_OF_DISTINCT_BYTES}}};

  EXPECT_EQ(decodeMessage<GameEnded>(encodeMessage(ended)), ended);
}

/**
 * Given a payload announcing five players and holding nothing more
 * When it is decoded
 * Then a TooManyPlayersException is raised before any player is read
 */
TEST(GameEnded, RefusesToReadFivePlayersBeforeReadingThem) {
  const auto bytes = byteSequence({0x05});

  EXPECT_THROW((void)decodeMessage<GameEnded>(bytes), TooManyPlayersException);
}

/**
 * Given a payload listing player 3 twice
 * When it is decoded
 * Then a DuplicatePlayerNumberException is raised
 */
TEST(GameEnded, RefusesToReadTheSamePlayerTwice) {
  const auto bytes = byteSequence(
      {0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<GameEnded>(bytes),
               DuplicatePlayerNumberException);
}

/**
 * Given the end of a game listing player 5
 * When it is encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(GameEnded, RefusesToWriteAPlayerNumberOutsideOneToFour) {
  const GameEnded ended{.players = {{.playerNumber = 5, .score = 0}}};

  EXPECT_THROW((void)encodeMessage(ended), InvalidFieldValueException);
}

/**
 * Given a payload announcing one player and holding only three bytes of its
 * score
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(GameEnded, RefusesAScoreCutShort) {
  const auto bytes = byteSequence({0x01, 0x01, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<GameEnded>(bytes), BufferUnderflowException);
}

/**
 * Given a whole end of game followed by one more byte
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(GameEnded, RefusesBytesAfterTheLastPlayer) {
  const auto bytes = byteSequence({0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<GameEnded>(bytes), TrailingBytesException);
}

}  // namespace
}  // namespace rtype::network
