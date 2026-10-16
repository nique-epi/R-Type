#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <string_view>
#include <vector>
#include "EncodedMessage.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "GameEventEncoder.hpp"
#include "Logger.hpp"
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
 * recipients there were when the bus delivered the event. Nothing is sent or
 * scheduled when there is no recipient.
 *
 * An event the encoder refuses (a position that is not a number, an entity id
 * of 0, a player number outside 1 to 4) is skipped and logged as a warning:
 * nothing is sent, no copy is scheduled, and the next event goes out. It never
 * stops the tick, which would stop the server for every player.
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
  void subscribeTo(engine::EventBus& events, std::string_view eventName);

  template <typename Event>
  std::optional<EncodedMessage> tryEncode(const Event& event,
                                          std::string_view eventName);

  void broadcast(const EncodedMessage& message);
  void sendCopy(const std::vector<network::Endpoint>& recipients,
                network::MessageType type,
                const std::vector<std::byte>& payload) const;

  engine::TimerScheduler& timers_;
  const std::vector<network::Endpoint>& recipients_;
  std::function<network::PacketHeader(const network::Endpoint&)> headerFor_;
  std::function<void(network::OutgoingDatagram)> send_;
  GameEventEncoder encoder_;
  logging::Logger logger_;
};

}  // namespace rtype::server
