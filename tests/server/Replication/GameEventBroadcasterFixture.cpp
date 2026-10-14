#include "GameEventBroadcasterFixture.hpp"
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "GameEventTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

GameEventBroadcasterFixture::GameEventBroadcasterFixture()
    : recipients_{firstClient(), secondClient()},
      broadcaster_(
          events_, timers_, recipients_,
          [this](const network::Endpoint& recipient) {
            network::PacketHeader header;
            header.sessionToken = recipient.port;
            header.sequence = nextSequence_++;
            return header;
          },
          [this](network::OutgoingDatagram datagram) {
            sent_.push_back(std::move(datagram));
          }) {}

network::Endpoint GameEventBroadcasterFixture::firstClient() {
  return network::Endpoint{.address = "127.0.0.1", .port = FIRST_CLIENT_PORT};
}

network::Endpoint GameEventBroadcasterFixture::secondClient() {
  return network::Endpoint{.address = "127.0.0.1", .port = SECOND_CLIENT_PORT};
}

void GameEventBroadcasterFixture::setRecipients(
    std::vector<network::Endpoint> recipients) {
  recipients_ = std::move(recipients);
}

void GameEventBroadcasterFixture::dispatch() { events_.dispatch(); }

void GameEventBroadcasterFixture::advanceTimers(engine::Duration elapsed) {
  timers_.advance(elapsed);
}

const std::vector<network::OutgoingDatagram>&
GameEventBroadcasterFixture::sentDatagrams() const {
  return sent_;
}

}  // namespace rtype::server::testing
