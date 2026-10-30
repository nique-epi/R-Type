#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "Endpoint.hpp"
#include "IRoutedLink.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "Reception.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the client reads from the server: the datagrams its link hands
 * over, checked, and how long since the last one that counted.
 *
 * Only datagrams of this protocol and version whose messages add up to their
 * end are read, with one exception: a datagram whose header is of this
 * protocol with another version is reported as ReceptionKind::AnotherVersion,
 * for the attempt to end. Once a session is open, a datagram is read only with
 * the token of the session, then only if newer than the newest one read, so a
 * late LobbyState never undoes a newer one; a datagram with another token is
 * counted by wrongTokenCount(). Every datagram read restarts the silence.
 */
class SessionInbound {
 public:
  /** @param link Where the datagrams come from; it must outlive the
   * inbound side. */
  explicit SessionInbound(IRoutedLink& link);

  /** @brief From now on reads the datagrams of @p server, drops what the link
   * already held, and starts the silence over. */
  void listenTo(const network::Endpoint& server);

  /** @brief Counts @p elapsed of silence, before the datagrams are read. */
  void advance(engine::Duration elapsed);

  /** @returns Whether the server was silent for SESSION_SILENCE_LIMIT. */
  [[nodiscard]] bool isSilentTooLong() const;

  /** @brief Opens the session: from now on only the token of
   * @p acceptance and the sequences after its own are read. */
  void open(const network::PacketHeader& acceptance);

  /** @brief Closes the session: the next datagram is checked like one of an
   * attempt. */
  void close();

  /** @returns The datagrams received since the previous call, valid until
   * the next. */
  [[nodiscard]] std::span<const network::IncomingDatagram> drain();

  [[nodiscard]] Reception receive(const network::IncomingDatagram& datagram);

  /** @returns How many datagrams of the server were dropped since the
   * connection was built because they came after an acceptance with another
   * session token than the one it carried. */
  [[nodiscard]] std::size_t wrongTokenCount() const;

 private:
  /** @returns Whether a datagram of the server may be read: always before the
   * acceptance; then only with the token of the session, and newer than the
   * newest one read, which it becomes. */
  [[nodiscard]] bool passesSessionChecks(const network::PacketHeader& header);

  IRoutedLink* link_;
  bool open_{false};
  std::uint64_t sessionToken_{network::NO_SESSION_TOKEN};
  std::uint16_t newestReceivedSequence_{0};
  std::size_t wrongTokenCount_{0};
  engine::Duration sinceReceived_{};
  std::vector<network::IncomingDatagram> received_;
};

}  // namespace rtype::client
