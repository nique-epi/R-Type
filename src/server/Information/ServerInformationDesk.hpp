#pragma once

#include <chrono>
#include <span>
#include <string>
#include "IncomingDatagram.hpp"
#include "InformationConstants.hpp"
#include "NetworkConstants.hpp"
#include "RequestReader.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationDeskParts.hpp"
#include "ServerInformationRequest.hpp"
#include "ServerOccupancy.hpp"
#include "ThrottledLog.hpp"

namespace rtype::server {

/**
 * @brief Answers the information requests among the datagrams the server
 * received, as the Server information section of the protocol page
 * describes: each with a ServerInformation sent back to its sender, outside
 * any session, and nothing kept.
 *
 * A datagram is read by a RequestReader: only when its header carries this
 * protocol and version and its messages add up to its end; only its first
 * ServerInformationRequest is answered, so a datagram never gets more than one
 * answer. The request is padded to the size of the largest answer, so its
 * sender never gets more bytes than it sent. A datagram that is not read, or
 * whose first request is refused, gets no answer; it is logged at the debug
 * level with its sender and why, at most once per REJECTION_LINE_INTERVAL (see
 * ThrottledLog).
 *
 * The answer echoes the sentAt of the request, with the time from the arrival
 * of its datagram to the answer as heldFor, so the asker leaves out the time
 * the request waited for the tick, as the echo of a Heartbeat does.
 */
class ServerInformationDesk {
 public:
  explicit ServerInformationDesk(ServerInformationDeskParts parts);

  /**
   * @brief Answers the requests among @p datagrams, oldest first, telling each
   * @p occupancy. An exception answering one (any std::exception, from the
   * sending) is a fault of the server, not of the datagram: it is logged at the
   * error level with its sender at most once per REJECTION_LINE_INTERVAL, and
   * the next one is answered.
   * @param now On the clock that stamped IncomingDatagram::receivedAt.
   */
  void answer(std::span<const network::IncomingDatagram> datagrams,
              const ServerOccupancy& occupancy, std::chrono::nanoseconds now);

 private:
  /** @returns The answer to @p request, held for @p held, telling
   * @p occupancy. */
  [[nodiscard]] network::ServerInformation informationFor(
      const network::ServerInformationRequest& request,
      std::chrono::nanoseconds held, const ServerOccupancy& occupancy) const;

  ServerInformationDeskParts parts_;
  RequestReader<network::ServerInformationRequest> requests_{
      std::string{INFORMATION_LOGGER_NAME}, NO_INFORMATION_REQUEST_DROP_REASON};
  logging::ThrottledLog failureLog_{std::string{INFORMATION_LOGGER_NAME},
                                    network::REJECTION_LINE_INTERVAL};
};

}  // namespace rtype::server
