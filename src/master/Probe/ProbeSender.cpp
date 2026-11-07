#include "ProbeSender.hpp"
#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <utility>
#include "Endpoint.hpp"
#include "MasterConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::master {

ProbeSender::ProbeSender()
    : failureLog_(std::string{MASTER_LOGGER_NAME},
                  network::REJECTION_LINE_INTERVAL) {}

ProbeSender::~ProbeSender() { network_.stop(); }

std::uint16_t ProbeSender::open() {
  socket_.emplace(
      network_,
      network::Endpoint{.address = std::string{network::ANY_IPV4_ADDRESS},
                        .port = network::ANY_PORT});
  const std::uint16_t port = socket_->localEndpoint().port;
  try {
    probeWire_ = std::jthread{[this] { network_.runUntilStopped(); }};
  } catch (...) {
    socket_.reset();
    throw;
  }
  return port;
}

void ProbeSender::close() {
  if (!probeWire_.joinable()) {
    return;
  }
  network_.stop();
  probeWire_.join();
  socket_.reset();
}

void ProbeSender::sendProbe(network::OutgoingDatagram probe) {
  network_.post([this, sent = std::move(probe)] { sendNow(sent); });
}

void ProbeSender::sendNow(const network::OutgoingDatagram& probe) {
  const auto now = std::chrono::steady_clock::now();
  if (!socket_.has_value() || !throttle_.allows(probe.destination, now)) {
    return;
  }
  try {
    socket_->send(probe.destination, probe.payload);
  } catch (const network::NetworkException& error) {
    failureLog_.error(now.time_since_epoch(), "a probe to ",
                      probe.destination.address, " port ",
                      probe.destination.port, " was not sent: ", error.what());
  }
}

}  // namespace rtype::master
