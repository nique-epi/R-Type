#pragma once

#include <cstddef>
#include <span>
#include <vector>
#include "ISessionOutbox.hpp"
#include "MessageType.hpp"
#include "SentToServer.hpp"

/**
 * @brief Session outbox double that writes down every message sent to the
 * server, in order, and whose acknowledgement is never due.
 */
class RecordingSessionOutbox : public rtype::client::ISessionOutbox {
 public:
  void sendToServer(rtype::network::MessageType type,
                    std::span<const std::byte> payload) override;
  [[nodiscard]] bool isAcknowledgementDue() const override;

  /** @returns The messages sent, oldest first. */
  [[nodiscard]] const std::vector<SentToServer>& sent() const;

 private:
  std::vector<SentToServer> sent_;
};
