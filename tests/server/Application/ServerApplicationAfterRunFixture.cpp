#include "ServerApplicationAfterRunFixture.hpp"
#include <cstddef>
#include <future>
#include <optional>
#include <span>
#include <string>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"
#include "ServerApplication.hpp"
#include "ServerTestConstants.hpp"
#include "TimeConstants.hpp"

namespace {

rtype::network::Endpoint loopbackOnAnyPort() {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
      .port = rtype::network::ANY_PORT};
}

}  // namespace

ServerApplicationAfterRunFixture::ServerApplicationAfterRunFixture()
    : server_(loopbackOnAnyPort(),
              []([[maybe_unused]] const rtype::server::ServerApplication::
                     ReceivedDatagrams& received) {}),
      client_(clientContext_, loopbackOnAnyPort()),
      firstReceived_(firstDatagram_.get_future()) {}

void ServerApplicationAfterRunFixture::TearDown() { clientContext_.stop(); }

rtype::server::ServerApplication& ServerApplicationAfterRunFixture::server() {
  return server_;
}

void ServerApplicationAfterRunFixture::runThenStop() {
  const std::jthread runner{[this] { server_.run(); }};
  std::this_thread::sleep_for(rtype::engine::SIMULATION_TICK_DURATION *
                              SERVER_RUN_TICKS);
  server_.stop();
}

void ServerApplicationAfterRunFixture::listenOnClient() {
  client_.startReceiving(
      [this]([[maybe_unused]] const rtype::network::Endpoint& sender,
             std::span<const std::byte> payload) {
        if (!delivered_.exchange(true)) {
          firstDatagram_.set_value(
              std::vector<std::byte>(payload.begin(), payload.end()));
        }
      });
  clientThread_ = std::jthread{[this] { clientContext_.run(); }};
}

rtype::network::Endpoint ServerApplicationAfterRunFixture::clientEndpoint()
    const {
  return client_.localEndpoint();
}

std::optional<std::vector<std::byte>>
ServerApplicationAfterRunFixture::firstReceivedByClient() {
  if (firstReceived_.wait_for(THREAD_TIMEOUT) != std::future_status::ready) {
    return std::nullopt;
  }
  return firstReceived_.get();
}
