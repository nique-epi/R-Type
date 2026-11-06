#include <gtest/gtest.h>
#include "GameChatFixtures.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceTestConstants.hpp"
#include "MessageType.hpp"

namespace rtype::server::testing {

/**
 * Given two seated players in the waiting room
 * When a datagram holding a ChatLine arrives from the first one
 * Then both get a PlayerChatLine: the handler is registered, the chat module is
 * in the list of the game, and the line goes through the checks of a real
 * datagram
 */
TEST_F(GameInstanceTest, RelaysAChatLineThroughARealDatagram) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  queue(chatLineFrom(FIRST_PLAYER));

  tick();

  EXPECT_EQ(
      ofType(network::MessageType::PlayerChatLine, sentTo(FIRST_PLAYER)).size(),
      1U);
  EXPECT_EQ(ofType(network::MessageType::PlayerChatLine, sentTo(SECOND_PLAYER))
                .size(),
            1U);
}

/**
 * Given two seated players
 * When a datagram from the first one holds a Disconnection, then a ChatLine
 * Then the second gets no PlayerChatLine: the line of an address with no seat
 * any more goes nowhere
 */
TEST_F(GameInstanceTest, DropsTheLineOfAPlayerWhoLeftInTheSameDatagram) {
  join(FIRST_PLAYER);
  join(SECOND_PLAYER);
  queue(disconnectionThenChatLineFrom(FIRST_PLAYER));

  tick();

  EXPECT_TRUE(
      ofType(network::MessageType::PlayerChatLine, sentTo(SECOND_PLAYER))
          .empty());
}

}  // namespace rtype::server::testing
