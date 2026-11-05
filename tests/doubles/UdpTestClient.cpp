#include "UdpTestClient.hpp"
#include <chrono>
#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "DatagramCompression.hpp"
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
        std::vector<std::byte> payload(bytes.begin(), bytes.end());
        if (rtype::network::isCompressed(bytes)) {
          std::optional<std::vector<std::byte>> plain =
              rtype::network::plainForm(bytes);
          if (!plain.has_value()) {
            return;
          }
          payload = std::move(*plain);
        }
        {
          const std::scoped_lock lock{mutex_};
          received_.push_back(
              ReceivedAt{.payload = std::move(payload),
                         .time = std::chrono::steady_clock::now()});
        }
        arrived_.notify_all();
      });
  runner_ = std::jthread{[this] { context_.run(); }};
}

UdpTestClient::~UdpTestClient() { context_.stop(); }

void UdpTestClient::send(const rtype::network::Endpoint& server,
                         const std::vector<std::byte>& datagram) {
  const auto outgoing = std::make_shared<
      const std::pair<rtype::network::Endpoint, std::vector<std::byte>>>(
      server, datagram);
  context_.post([this, outgoing] {
    try {
      socket_.send(outgoing->first, outgoing->second);
    } catch (...) {
      ++failedSends_;
    }
  });
}

std::size_t UdpTestClient::failedSendCount() const { return failedSends_; }

std::vector<ReceivedAt> UdpTestClient::waitForDatagrams(
    std::size_t count, std::chrono::milliseconds timeout) {
  std::unique_lock lock{mutex_};
  arrived_.wait_for(lock, timeout,
                    [this, count] { return received_.size() >= count; });
  return received_;
}

rtype::network::Endpoint UdpTestClient::localEndpoint() const {
  return socket_.localEndpoint();
}
