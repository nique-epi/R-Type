#pragma once

#include <functional>
#include <optional>
#include <string_view>
#include <vector>
#include "EncodedMessage.hpp"
#include "EventBus.hpp"
#include "EventMessage.hpp"
#include "GameEventBroadcasterParts.hpp"
#include "GameEventEncoder.hpp"
#include "Logger.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {

/**
 * @brief Hands the events of the match to the players of the game, as the
 * messages GameEventEncoder writes for a numbered GameEvent and
 * eventMessageOf() for any other event: once when the message says a later
 * state message corrects its loss, guaranteed otherwise.
 *
 * An event the encoder refuses (a position that is not a number, an entity id
 * of 0, a player number outside 1 to 4) is skipped and logged as a warning:
 * nothing is sent, and the next event goes out. It never stops the tick, which
 * would stop the server for every player.
 *
 * The broadcaster keeps the handle of each of its subscriptions and gives them
 * back to the bus when it is destroyed, so the bus never calls it after that.
 */
class GameEventBroadcaster {
 public:
  explicit GameEventBroadcaster(GameEventBroadcasterParts parts);

  GameEventBroadcaster(const GameEventBroadcaster&) = delete;
  GameEventBroadcaster& operator=(const GameEventBroadcaster&) = delete;
  GameEventBroadcaster(GameEventBroadcaster&&) = delete;
  GameEventBroadcaster& operator=(GameEventBroadcaster&&) = delete;
  ~GameEventBroadcaster();

 private:
  template <typename Event>
  void subscribeTo(std::string_view eventName);

  template <typename Event>
  std::optional<EventMessage> tryEncode(const Event& event,
                                        std::string_view eventName);

  void broadcast(const EventMessage& event) const;

  engine::EventBus& events_;
  std::function<void(const network::EncodedMessage&)> sendToEveryPlayer_;
  std::function<void(const network::EncodedMessage&)> guaranteeToEveryPlayer_;
  GameEventEncoder encoder_;
  logging::Logger logger_;
  std::vector<engine::SubscriptionHandle> subscriptions_;
};

}  // namespace rtype::server
