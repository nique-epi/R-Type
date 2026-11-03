#include "SessionInbound.hpp"
#include <cstddef>
#include <span>
#include <utility>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "IRoutedLink.hpp"
#include "IncomingDatagram.hpp"
#include "MessageFrame.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "Reception.hpp"
#include "ReceptionKind.hpp"
#include "SequenceOrder.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

SessionInbound::SessionInbound(IRoutedLink& link) : link_(&link) {}

void SessionInbound::listenTo(const network::Endpoint& server) {
  link_->listenTo(server);
  sinceReceived_ = engine::Duration::zero();
}

void SessionInbound::advance(engine::Duration elapsed) {
  sinceReceived_ += elapsed;
}

bool SessionInbound::isSilentTooLong() const {
  return sinceReceived_ >= network::SESSION_SILENCE_LIMIT;
}

void SessionInbound::open(const network::PacketHeader& acceptance) {
  open_ = true;
  sessionToken_ = acceptance.sessionToken;
  newestReceivedSequence_ = acceptance.sequence;
}

void SessionInbound::close() {
  open_ = false;
  sessionToken_ = network::NO_SESSION_TOKEN;
  newestReceivedSequence_ = 0;
}

std::span<const network::IncomingDatagram> SessionInbound::drain() {
  received_.clear();
  link_->drainReceived(received_);
  return received_;
}

Reception SessionInbound::receive(const network::IncomingDatagram& datagram) {
  try {
    const network::PacketHeader header =
        network::DatagramCodec::decodeHeader(datagram.payload);
    if (network::DatagramCodec::speaksAnotherVersion(header)) {
      return Reception{.kind = ReceptionKind::AnotherVersion,
                       .header = header,
                       .messages = {}};
    }
    if (!network::DatagramCodec::speaksThisProtocol(header) ||
        !passesSessionChecks(header)) {
      return Reception{
          .kind = ReceptionKind::Ignored, .header = header, .messages = {}};
    }
    std::vector<network::MessageFrame> messages =
        network::DatagramCodec::splitMessages(datagram.payload);
    sinceReceived_ = engine::Duration::zero();
    return Reception{.kind = ReceptionKind::Read,
                     .header = header,
                     .messages = std::move(messages)};
  } catch (const network::MalformedDatagramException&) {
    return Reception{
        .kind = ReceptionKind::Ignored, .header = {}, .messages = {}};
  }
}

std::size_t SessionInbound::wrongTokenCount() const { return wrongTokenCount_; }

bool SessionInbound::passesSessionChecks(const network::PacketHeader& header) {
  if (!open_) {
    return true;
  }
  if (header.sessionToken != sessionToken_) {
    ++wrongTokenCount_;
    return false;
  }
  if (!network::isNewerSequence(header.sequence, newestReceivedSequence_)) {
    return false;
  }
  newestReceivedSequence_ = header.sequence;
  return true;
}

}  // namespace rtype::client
