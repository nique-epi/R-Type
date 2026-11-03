#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"
#include "ISessionOutbox.hpp"
#include "MessageType.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

/**
 * @brief Everything the client sends the server goes through here: each
 * datagram carries the token of the session, 0 before it, and the next
 * sequence, which starts at 0 with each attempt and counts every datagram.
 *
 * It can be copied: the copy goes on numbering from where the original stood,
 * which is how the copies of a Disconnection leave under the token and the
 * sequences of the session they leave.
 */
class SessionOutbox final : public ISessionOutbox {
 public:
  /** @param link Where the datagrams go; it must outlive the outbox. */
  explicit SessionOutbox(IDatagramLink& link);

  /** @brief From now on sends to @p server, numbering from 0 again. */
  void aim(const network::Endpoint& server);

  /** @brief From now on writes @p token in every datagram. */
  void openSession(std::uint64_t token);

  /** @brief From now on writes NO_SESSION_TOKEN in every datagram. */
  void closeSession();

  void sendToServer(network::MessageType type,
                    std::span<const std::byte> payload) override;

  [[nodiscard]] const network::Endpoint& server() const;

 private:
  /** @returns A header carrying the token of the session and the next
   * sequence, which it uses up. */
  [[nodiscard]] network::PacketHeader nextHeader();

  IDatagramLink* link_;
  network::Endpoint server_;
  std::uint64_t sessionToken_{network::NO_SESSION_TOKEN};
  std::uint16_t nextSequence_{0};
};

}  // namespace rtype::client
