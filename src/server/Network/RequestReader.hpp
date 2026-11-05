#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "MessagePayload.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "ThrottledLog.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Reads the request a datagram sent outside any session holds, for the
 * parts of the server that answer one: the first message of type Message of a
 * datagram whose header is of this protocol and version and whose messages add
 * up (network::DatagramCodec::firstMessage()).
 *
 * A datagram it cannot read, or whose first request its reader refuses, is
 * dropped: logged at the debug level with its sender and why, at most once per
 * REJECTION_LINE_INTERVAL (see ThrottledLog), under the logger name it is
 * given. Not thread-safe.
 */
template <network::DecodableMessage Message>
class RequestReader {
 public:
  /**
   * @param loggerName The logger its lines are written under.
   * @param noRequestReason Why a datagram without such a request is dropped,
   * as its line says; it must outlive the reader.
   */
  RequestReader(const std::string& loggerName, std::string_view noRequestReason)
      : noRequestReason_(noRequestReason),
        dropLog_(loggerName, network::REJECTION_LINE_INTERVAL) {}

  /** @returns The first request of @p datagram, or nothing when it is dropped,
   * which is then logged at @p now. */
  [[nodiscard]] std::optional<Message> read(
      const network::IncomingDatagram& datagram, engine::Duration now) {
    std::optional<Message> request;
    try {
      request = network::DatagramCodec::firstMessage<Message>(datagram.payload);
    } catch (const network::NetworkException& error) {
      drop(datagram.sender, error.what(), now);
      return std::nullopt;
    }
    if (!request.has_value()) {
      drop(datagram.sender, noRequestReason_, now);
    }
    return request;
  }

  /** @brief Logs that the datagram of @p sender was dropped for @p reason. */
  void drop(const network::Endpoint& sender, std::string_view reason,
            engine::Duration now) {
    dropLog_.debug(now, "dropped a datagram from ", sender.address, " port ",
                   sender.port, ": ", reason);
  }

 private:
  std::string_view noRequestReason_;
  logging::ThrottledLog dropLog_;
};

}  // namespace rtype::server
