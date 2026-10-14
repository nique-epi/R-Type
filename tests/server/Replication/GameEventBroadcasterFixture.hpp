#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <utility>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "GameEventBroadcaster.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"

namespace rtype::server::testing {

/**
 * @brief A broadcaster on a real event bus and a real timer scheduler, with
 * two clients, and the datagrams it sent, in order. Each header it is asked
 * for gets the next sequence number, so a test can tell the datagrams apart.
 */
class GameEventBroadcasterFixture : public ::testing::Test {
 public:
  GameEventBroadcasterFixture();

 protected:
  [[nodiscard]] static network::Endpoint firstClient();
  [[nodiscard]] static network::Endpoint secondClient();

  /**
   * @brief Replaces the clients of the game.
   */
  void setRecipients(std::vector<network::Endpoint> recipients);

  /**
   * @brief Queues @p event on the bus, without delivering it.
   */
  template <typename Event>
  void publish(Event event) {
    events_.publish(std::move(event));
  }

  /**
   * @brief Delivers the queued events to the broadcaster.
   */
  void dispatch();

  /**
   * @brief Publishes @p event and delivers it.
   */
  template <typename Event>
  void publishAndDispatch(Event event) {
    publish(std::move(event));
    dispatch();
  }

  /**
   * @brief Moves the timers forward by @p elapsed, sending the copies that
   * come due.
   */
  void advanceTimers(engine::Duration elapsed);

  /**
   * @returns The datagrams the broadcaster handed to its send function, in
   * order.
   */
  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sentDatagrams()
      const;

  /**
   * @returns The messages of type Message that went to @p destination, in
   * the order they were sent.
   */
  template <network::DecodableMessage Message>
  [[nodiscard]] std::vector<Message> receivedBy(
      const network::Endpoint& destination) const {
    std::vector<Message> received;
    for (const network::OutgoingDatagram& datagram : sent_) {
      if (!(datagram.destination == destination)) {
        continue;
      }
      for (const network::MessageFrame& frame :
           network::DatagramCodec::splitMessages(datagram.payload)) {
        if (frame.type == static_cast<std::uint8_t>(Message::TYPE)) {
          received.push_back(network::decodeMessage<Message>(frame.payload));
        }
      }
    }
    return received;
  }

 private:
  engine::EventBus events_;
  engine::TimerScheduler timers_;
  std::vector<network::Endpoint> recipients_;
  std::vector<network::OutgoingDatagram> sent_;
  std::uint16_t nextSequence_{1};
  GameEventBroadcaster broadcaster_;
};

}  // namespace rtype::server::testing
