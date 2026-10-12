#include "UdpSocketFixture.hpp"
#include <array>
#include <asio/buffer.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/udp.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <system_error>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "NetworkTestConstants.hpp"
#include "UdpSocket.hpp"

using rtype::network::DatagramHandler;
using rtype::network::Endpoint;
using rtype::network::NetworkContext;
using rtype::network::UdpSocket;

NetworkContext& UdpSocketFixture::context() { return context_; }

Endpoint UdpSocketFixture::loopbackOnPort(std::uint16_t port) {
  return Endpoint{.address = std::string{LOOPBACK_ADDRESS}, .port = port};
}

std::vector<std::byte> UdpSocketFixture::makePayload(std::size_t size) {
  std::vector<std::byte> payload(size);
  for (std::size_t i = 0; i < size; ++i) {
    payload[i] = static_cast<std::byte>(static_cast<unsigned char>(i));
  }
  return payload;
}

DatagramHandler UdpSocketFixture::recordInto(
    std::vector<ReceivedDatagram>& received) {
  return
      [&received](const Endpoint& sender, std::span<const std::byte> payload) {
        received.push_back(ReceivedDatagram{
            .sender = sender, .payload = {payload.begin(), payload.end()}});
      };
}

bool UdpSocketFixture::runUntil(const std::function<bool()>& isDone) {
  const auto deadline = std::chrono::steady_clock::now() + DATAGRAM_TIMEOUT;
  while (!isDone()) {
    if (context_.ioContext().run_one_until(deadline) == 0) {
      return false;
    }
  }
  return true;
}

void UdpSocketFixture::runUntilIdle() {
  context_.ioContext().run_for(DATAGRAM_TIMEOUT);
}

void UdpSocketFixture::openSocketOn(const Endpoint& localEndpoint) {
  const UdpSocket socket{context_, localEndpoint};
}

asio::ip::udp::socket UdpSocketFixture::openClient() {
  return asio::ip::udp::socket{
      context_.ioContext(),
      asio::ip::udp::endpoint{
          asio::ip::make_address(std::string{LOOPBACK_ADDRESS}), ANY_PORT}};
}

void UdpSocketFixture::sendFromClient(asio::ip::udp::socket& client,
                                      std::uint16_t port,
                                      std::span<const std::byte> payload) {
  client.send_to(
      asio::buffer(payload.data(), payload.size()),
      asio::ip::udp::endpoint{
          asio::ip::make_address(std::string{LOOPBACK_ADDRESS}), port});
}

std::optional<std::vector<std::byte>> UdpSocketFixture::receiveOnClient(
    asio::ip::udp::socket& client) {
  std::array<std::byte, rtype::network::RECEIVE_BUFFER_SIZE> buffer{};
  asio::ip::udp::endpoint sender;
  std::optional<std::size_t> byteCount;
  client.async_receive_from(
      asio::buffer(buffer), sender,
      [&byteCount](const std::error_code& error, std::size_t count) {
        if (!error) {
          byteCount = count;
        }
      });
  runUntil([&byteCount] { return byteCount.has_value(); });
  if (!byteCount.has_value()) {
    client.cancel();
    return std::nullopt;
  }
  const auto received = std::span<const std::byte>{buffer}.first(*byteCount);
  return std::vector<std::byte>{received.begin(), received.end()};
}
