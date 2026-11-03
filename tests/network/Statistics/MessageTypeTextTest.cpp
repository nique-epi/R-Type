#include <gtest/gtest.h>
#include <cstdint>
#include "MessageType.hpp"
#include "MessageTypeText.hpp"

using rtype::network::MessageType;
using rtype::network::messageTypeText;

/**
 * Given every message of the protocol page
 * When its text is asked
 * Then it is its name
 */
TEST(MessageTypeText, EveryMessageHasItsName) {
  const auto textOf = [](MessageType type) {
    return messageTypeText(static_cast<std::uint8_t>(type));
  };

  EXPECT_EQ(textOf(MessageType::ConnectionRequest), "ConnectionRequest");
  EXPECT_EQ(textOf(MessageType::ConnectionAccepted), "ConnectionAccepted");
  EXPECT_EQ(textOf(MessageType::ConnectionRefused), "ConnectionRefused");
  EXPECT_EQ(textOf(MessageType::Disconnection), "Disconnection");
  EXPECT_EQ(textOf(MessageType::Heartbeat), "Heartbeat");
  EXPECT_EQ(textOf(MessageType::ReadyState), "ReadyState");
  EXPECT_EQ(textOf(MessageType::LobbyState), "LobbyState");
  EXPECT_EQ(textOf(MessageType::GameStarted), "GameStarted");
  EXPECT_EQ(textOf(MessageType::PlayerInput), "PlayerInput");
  EXPECT_EQ(textOf(MessageType::WorldState), "WorldState");
  EXPECT_EQ(textOf(MessageType::EntitySpawned), "EntitySpawned");
  EXPECT_EQ(textOf(MessageType::EntityDestroyed), "EntityDestroyed");
  EXPECT_EQ(textOf(MessageType::GameEvent), "GameEvent");
  EXPECT_EQ(textOf(MessageType::PlayerLeft), "PlayerLeft");
  EXPECT_EQ(textOf(MessageType::GameEnded), "GameEnded");
  EXPECT_EQ(textOf(MessageType::ConnectionChallenge), "ConnectionChallenge");
  EXPECT_EQ(textOf(MessageType::GameLoaded), "GameLoaded");
}

/**
 * Given the type bytes next to the first and the last message, and the largest
 * When their text is asked
 * Then it is the number: no message has them
 */
TEST(MessageTypeText, AByteNoMessageHasGivesItsNumber) {
  EXPECT_EQ(messageTypeText(0), "type-0");
  EXPECT_EQ(messageTypeText(18), "type-18");
  EXPECT_EQ(messageTypeText(255), "type-255");
}
