#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <span>
#include <utility>
#include <vector>
#include "ISessionOutbox.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"

/**
 * @brief Session outbox double: it writes down the type and the payload of
 * every message sent, in order, and sends nothing.
 */
class RecordingSessionOutbox final : public rtype::client::ISessionOutbox {
 public:
  void sendToServer(rtype::network::MessageType type,
                    std::span<const std::byte> payload) override;

  /** @returns How many messages of @p type were sent. */
  [[nodiscard]] std::size_t sentCountOf(rtype::network::MessageType type) const;

  /** @returns The newest message sent, read as a Message; the test fails
   * when none was sent or it is of another type. */
  template <rtype::network::DecodableMessage Message>
  [[nodiscard]] Message lastSent() const {
    if (sent_.empty() || sent_.back().first != Message::TYPE) {
      ADD_FAILURE() << "the newest message sent is not of the type asked";
      return Message{};
    }
    return rtype::network::decodeMessage<Message>(sent_.back().second);
  }

 private:
  std::vector<std::pair<rtype::network::MessageType, std::vector<std::byte>>>
      sent_;
};
