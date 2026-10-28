#include "LoopbackPeer.hpp"
#include <asio/buffer.hpp>
#include <asio/ip/address_v4.hpp>
#include <asio/ip/udp.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <system_error>
#include <thread>
#include "Endpoint.hpp"
#include "FloodConstants.hpp"
#include "IncomingDatagram.hpp"

namespace rtype::tools {

namespace {

asio::ip::udp::endpoint loopbackOn(std::uint16_t port) {
  return asio::ip::udp::endpoint{asio::ip::address_v4::loopback(), port};
}

}  // namespace

LoopbackPeer::LoopbackPeer(std::uint16_t port)
    : socket_(context_, loopbackOn(port)) {
  socket_.non_blocking(true);
}

bool LoopbackPeer::send(std::uint16_t port,
                        std::span<const std::byte> datagram) {
  std::error_code error;
  socket_.send_to(asio::buffer(datagram.data(), datagram.size()),
                  loopbackOn(port), 0, error);
  return !error;
}

std::optional<network::IncomingDatagram> LoopbackPeer::receive(
    std::chrono::milliseconds timeout) {
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  std::optional<network::IncomingDatagram> datagram = receiveWaiting();
  while (!datagram.has_value() && std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(POLL_INTERVAL);
    datagram = receiveWaiting();
  }
  return datagram;
}

std::size_t LoopbackPeer::drainFrom(std::uint16_t port) {
  std::size_t count = 0;
  for (std::optional<network::IncomingDatagram> datagram = receiveWaiting();
       datagram.has_value(); datagram = receiveWaiting()) {
    if (datagram->sender.port == port) {
      ++count;
    }
  }
  return count;
}

std::optional<network::IncomingDatagram> LoopbackPeer::receiveWaiting() {
  asio::ip::udp::endpoint sender;
  std::error_code error;
  const std::size_t byteCount =
      socket_.receive_from(asio::buffer(buffer_), sender, 0, error);
  if (error) {
    return std::nullopt;
  }
  const std::span<const std::byte> received =
      std::span{buffer_}.first(byteCount);
  return network::IncomingDatagram{
      .sender = network::Endpoint{.address = sender.address().to_string(),
                                  .port = sender.port()},
      .payload = {received.begin(), received.end()}};
}

}  // namespace rtype::tools
