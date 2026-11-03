#include "SessionOutbox.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IDatagramLink.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

SessionOutbox::SessionOutbox(IDatagramLink& link) : link_(&link) {}

void SessionOutbox::aim(const network::Endpoint& server) {
  server_ = server;
  nextSequence_ = 0;
}

void SessionOutbox::openSession(std::uint64_t token) { sessionToken_ = token; }

void SessionOutbox::closeSession() {
  sessionToken_ = network::NO_SESSION_TOKEN;
}

void SessionOutbox::sendToServer(network::MessageType type,
                                 std::span<const std::byte> payload) {
  link_->send(network::OutgoingDatagram{
      .destination = server_,
      .payload = network::DatagramCodec::encode(nextHeader(), type, payload)});
}

const network::Endpoint& SessionOutbox::server() const { return server_; }

network::PacketHeader SessionOutbox::nextHeader() {
  network::PacketHeader header;
  header.sessionToken = sessionToken_;
  header.sequence = nextSequence_;
  ++nextSequence_;
  return header;
}

}  // namespace rtype::client
