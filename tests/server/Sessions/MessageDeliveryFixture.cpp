#include "MessageDeliveryFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EncodedMessage.hpp"
#include "GuaranteedMessageTestConstants.hpp"
#include "MessageDelivery.hpp"
#include "MessageDeliveryTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "SessionsFixtures.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

MessageDeliveryFixture::MessageDeliveryFixture()
    : send_([this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {
  addPlayer(roster_, FIRST_PLAYER);
  addPlayer(roster_, SECOND_PLAYER);
}

PlayerRoster& MessageDeliveryFixture::roster() { return roster_; }

void MessageDeliveryFixture::sendOnce(const network::EncodedMessage& message) {
  sendToEveryPlayer(roster_, send_, message);
}

void MessageDeliveryFixture::guarantee(const network::EncodedMessage& message) {
  guaranteeToEveryPlayer(roster_, send_, message);
}

void MessageDeliveryFixture::endTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    ageGuaranteedMessages(roster_, engine::SIMULATION_TICK_DURATION);
    resendGuaranteedMessages(roster_, send_);
  }
}

std::vector<network::OutgoingDatagram> MessageDeliveryFixture::sentTo(
    std::size_t index) const {
  const network::Endpoint destination = originOf(index).sender;
  std::vector<network::OutgoingDatagram> received;
  for (const network::OutgoingDatagram& datagram : sent_) {
    if (datagram.destination == destination) {
      received.push_back(datagram);
    }
  }
  return received;
}

void MessageDeliveryFixture::acknowledgeFrom(std::size_t index,
                                             std::uint16_t sequence) {
  network::PacketHeader header;
  header.flags = PROTOCOL_ACKNOWLEDGES_FLAG;
  header.acknowledgedSequence = sequence;
  roster_.find(originOf(index).sender)->unacknowledged.acknowledge(header);
}

}  // namespace rtype::server::testing
