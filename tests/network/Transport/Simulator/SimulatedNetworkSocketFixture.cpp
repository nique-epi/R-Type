#include "SimulatedNetworkSocketFixture.hpp"
#include <asio/io_context.hpp>
#include <chrono>
#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "NetworkTestConstants.hpp"
#include "RecordingDatagramSocket.hpp"
#include "SimulatedNetworkSocket.hpp"

std::unique_ptr<rtype::network::SimulatedNetworkSocket>
SimulatedNetworkSocketFixture::overRecording(
    const rtype::network::NetworkConditions& conditions) {
  auto recording = std::make_unique<RecordingDatagramSocket>();
  recording_ = recording.get();
  return std::make_unique<rtype::network::SimulatedNetworkSocket>(
      context_, std::move(recording), conditions);
}

RecordingDatagramSocket& SimulatedNetworkSocketFixture::recording() {
  return *recording_;
}

rtype::network::NetworkContext& SimulatedNetworkSocketFixture::context() {
  return context_;
}

bool SimulatedNetworkSocketFixture::runOneHandler() {
  return context_.ioContext().run_one_for(DATAGRAM_TIMEOUT) == 1;
}

void SimulatedNetworkSocketFixture::runAtMostOneHandlerFor(
    std::chrono::milliseconds duration) {
  static_cast<void>(context_.ioContext().run_one_for(duration));
}

void SimulatedNetworkSocketFixture::runUntilIdle() { context_.run(); }

rtype::network::DatagramHandler SimulatedNetworkSocketFixture::recordInto(
    std::vector<rtype::network::IncomingDatagram>& received) {
  return [&received](const rtype::network::Endpoint& sender,
                     std::span<const std::byte> payload) {
    received.push_back(rtype::network::IncomingDatagram{
        .sender = sender, .payload = {payload.begin(), payload.end()}});
  };
}

rtype::network::Endpoint SimulatedNetworkSocketFixture::loopbackOnAnyPort() {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = rtype::network::ANY_PORT};
}
