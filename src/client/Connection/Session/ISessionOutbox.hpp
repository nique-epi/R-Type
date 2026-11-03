#pragma once

#include <cstddef>
#include <span>
#include "MessagePayload.hpp"
#include "MessageType.hpp"

namespace rtype::client {

/**
 * @brief Where the parts of a session send what they have to say to the
 * server: every datagram goes out under the token of the session and the next
 * sequence, however many parts send.
 */
class ISessionOutbox {
 public:
  virtual ~ISessionOutbox() = default;

  /**
   * @brief Sends the server one datagram holding @p payload as a message of
   * type @p type.
   */
  virtual void sendToServer(network::MessageType type,
                            std::span<const std::byte> payload) = 0;
};

/**
 * @brief Sends the server one datagram holding @p message.
 * @throws What encodeMessage() throws for @p message, before anything is
 * sent or any sequence is used.
 */
template <network::EncodableMessage Message>
void sendMessage(ISessionOutbox& outbox, const Message& message) {
  outbox.sendToServer(Message::TYPE, network::encodeMessage(message));
}

}  // namespace rtype::client
