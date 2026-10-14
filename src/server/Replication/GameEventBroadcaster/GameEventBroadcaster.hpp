#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include "EncodedMessage.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "GameEventEncoder.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "TimerScheduler.hpp"

namespace rtype::server {

/**
 * @brief Sends the events of the match to every recipient, as the messages
 * GameEventEncoder writes, each one as many times as it asks.
 *
 * The copies after the first are sent REPEATED_MESSAGE_INTERVAL apart, to the
 * recipients there were when the event was published. Nothing is sent or
 * scheduled when there is no recipient.
 *
 * The broadcaster must outlive the last dispatch() of the event bus and the
 * last advance() of the timer scheduler: both call back into it.
 */
class GameEventBroadcaster {
 public:
  /**
   * @param events Where the events are published; the broadcaster subscribes
   * to it.
   * @param timers Runs the later copies.
   * @param recipients The clients of the game, kept up to date by whoever
   * owns the list. Must outlive the broadcaster.
   * @param headerFor Gives the header of the next datagram for a recipient:
   * its session token and its next sequence number. Must not be empty.
   * @param send Called for each datagram to send. Must not be empty.
   */
  GameEventBroadcaster(
      engine::EventBus& events, engine::TimerScheduler& timers,
      const std::vector<network::Endpoint>& recipients,
      std::function<network::PacketHeader(const network::Endpoint&)> headerFor,
      std::function<void(network::OutgoingDatagram)> send);

  GameEventBroadcaster(const GameEventBroadcaster&) = delete;
  GameEventBroadcaster& operator=(const GameEventBroadcaster&) = delete;
  GameEventBroadcaster(GameEventBroadcaster&&) = delete;
  GameEventBroadcaster& operator=(GameEventBroadcaster&&) = delete;
  ~GameEventBroadcaster() = default;

 private:
  template <typename Event>
  void subscribeTo(engine::EventBus& events) {
    events.subscribe<Event>(
        [this](const Event& event) { broadcast(encoder_.encode(event)); });
  }

  void broadcast(const EncodedMessage& message);
  void sendCopy(const std::vector<network::Endpoint>& recipients,
                network::MessageType type,
                const std::vector<std::byte>& payload) const;

  engine::TimerScheduler& timers_;
  const std::vector<network::Endpoint>& recipients_;
  std::function<network::PacketHeader(const network::Endpoint&)> headerFor_;
  std::function<void(network::OutgoingDatagram)> send_;
  GameEventEncoder encoder_;
};

}  // namespace rtype::server
