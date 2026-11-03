#pragma once

#include <functional>
#include "EncodedMessage.hpp"
#include "EventBus.hpp"

namespace rtype::server {

/** @brief What a GameEventBroadcaster is built from. */
struct GameEventBroadcasterParts {
  /** @brief Where the events are published; the broadcaster subscribes to
   * it. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief Sends a message once to every player of the game. Must not be
   * empty. */
  std::function<void(const network::EncodedMessage&)> sendToEveryPlayer;
  /** @brief Gives a message to every player of the game as a guaranteed
   * message. Must not be empty. */
  std::function<void(const network::EncodedMessage&)> guaranteeToEveryPlayer;
};

}  // namespace rtype::server
