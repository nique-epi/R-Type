#pragma once

#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "OrderedMessage.hpp"
#include "OrderedReceiver.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The ordered channel of the server, on the client: it takes in each
 * OrderedMessage the dispatcher reads, and hands the messages they carry to
 * their handlers on the same dispatcher, in the order the server numbered
 * them, once each (see OrderedReceiver). The messages a datagram carries
 * beside an OrderedMessage reach their handlers as they arrive, so a message
 * missing from the channel delays nothing else.
 *
 * Leaving the session forgets the messages held: a new session numbers from
 * the start.
 */
class OrderedInbox final : public ISessionObserver {
 public:
  /** @param dispatcher Where the handler of OrderedMessage is registered and
   * the carried messages are dispatched; it must outlive the inbox. */
  explicit OrderedInbox(network::MessageDispatcher& dispatcher);

  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;

  /** @brief Forgets the messages held and expects the first number again. */
  void sessionClosed() override;

  void sessionHeld(engine::Duration elapsed) override;

 private:
  void deliver(const network::MessageOrigin& origin,
               const network::OrderedMessage& envelope);

  network::MessageDispatcher* dispatcher_;
  network::OrderedReceiver receiver_;
};

}  // namespace rtype::client
