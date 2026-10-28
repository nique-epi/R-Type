#include "UdpTestClient.hpp"
#include <chrono>
#include <cstddef>
#include <mutex>
#include <span>
#include <string>
#include <thread>
#include <vector>
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"

UdpTestClient::UdpTestClient()
    : socket_(context_,
              rtype::network::Endpoint{
                  .address = std::string{rtype::network::LOOPBACK_IPV4_ADDRESS},
                  .port = rtype::network::ANY_PORT}) {
  socket_.startReceiving(
      [this]([[maybe_unused]] const rtype::network::Endpoint& sender,
             std::span<const std::byte> bytes) {
        {
          const std::scoped_lock lock{mutex_};
          received_.push_back(ReceivedAt{
              .payload = std::vector<std::byte>(bytes.begin(), bytes.end()),
              .time = std::chrono::steady_clock::now()});
        }
        arrived_.notify_all();
      });
  runner_ = std::jthread{[this] { context_.run(); }};
}

UdpTestClient::~UdpTestClient() {
  context_.stop();
  runner_.join();
}

void UdpTestClient::send(const rtype::network::Endpoint& server,
                         const std::vector<std::byte>& datagram) {
  context_.post([this, server, datagram] { socket_.send(server, datagram); });
}

std::vector<ReceivedAt> UdpTestClient::waitForDatagrams(
    std::size_t count, std::chrono::milliseconds timeout) {
  std::unique_lock lock{mutex_};
  arrived_.wait_for(lock, timeout,
                    [this, count] { return received_.size() >= count; });
  return received_;
}
