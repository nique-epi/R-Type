#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include "EncodedMessage.hpp"
#include "EventBus.hpp"
#include "GameEventBroadcaster.hpp"
#include "MessagePayload.hpp"

namespace rtype::server::testing {

/**
 * @brief A broadcaster on a real event bus, and the messages it handed to its
 * two functions, in order: those sent once to every player and those
 * guaranteed to every player.
 */
class GameEventBroadcasterFixture : public ::testing::Test {
 public:
  GameEventBroadcasterFixture();

 protected:
  /** @brief Queues @p event on the bus, without delivering it. */
  template <typename Event>
  void publish(Event event) {
    events_.publish(std::move(event));
  }

  /** @brief Delivers the queued events to the broadcaster. */
  void dispatch();

  /** @brief Publishes @p event and delivers it. */
  template <typename Event>
  void publishAndDispatch(Event event) {
    publish(std::move(event));
    dispatch();
  }

  /** @brief Destroys the broadcaster, as a game does when its module stops. */
  void destroyBroadcaster();

  /** @returns The messages of type Message sent once, in order. */
  template <network::DecodableMessage Message>
  [[nodiscard]] std::vector<Message> sentOnce() const {
    return decodedOf<Message>(sentOnce_);
  }

  /** @returns The messages of type Message guaranteed, in order. */
  template <network::DecodableMessage Message>
  [[nodiscard]] std::vector<Message> guaranteed() const {
    return decodedOf<Message>(guaranteed_);
  }

  /** @returns How many messages went out, once or guaranteed. */
  [[nodiscard]] std::size_t messageCount() const;

 private:
  template <network::DecodableMessage Message>
  [[nodiscard]] static std::vector<Message> decodedOf(
      const std::vector<network::EncodedMessage>& messages) {
    std::vector<Message> decoded;
    for (const network::EncodedMessage& message : messages) {
      if (message.type == Message::TYPE) {
        decoded.push_back(network::decodeMessage<Message>(message.payload));
      }
    }
    return decoded;
  }

  engine::EventBus events_;
  std::vector<network::EncodedMessage> sentOnce_;
  std::vector<network::EncodedMessage> guaranteed_;
  std::unique_ptr<GameEventBroadcaster> broadcaster_;
};

}  // namespace rtype::server::testing
