#include "SessionAcknowledgements.hpp"
#include <cstdint>
#include "GuaranteedDelivery.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "ReceivedSequences.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void SessionAcknowledgements::open(std::uint16_t acceptanceSequence) {
  sequences_ = network::ReceivedSequences::startingAt(acceptanceSequence);
  wait_.reset();
}

void SessionAcknowledgements::close() {
  sequences_ = network::ReceivedSequences{};
  wait_.reset();
}

bool SessionAcknowledgements::accept(const network::PacketHeader& header) {
  if (!sequences_.accept(header.sequence)) {
    return false;
  }
  if (network::holdsGuaranteedMessages(header) && !wait_.has_value()) {
    wait_ = engine::Duration::zero();
  }
  return true;
}

void SessionAcknowledgements::acknowledgeIn(network::PacketHeader& header) {
  sequences_.acknowledgeIn(header);
  wait_.reset();
}

void SessionAcknowledgements::advance(engine::Duration elapsed) {
  if (wait_.has_value()) {
    *wait_ += elapsed;
  }
}

bool SessionAcknowledgements::isAcknowledgementDue() const {
  return wait_.has_value() && *wait_ >= network::ACKNOWLEDGEMENT_DELAY;
}

}  // namespace rtype::client
