#include "DatagramDrops.hpp"
#include <atomic>
#include <cstddef>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "Endpoint.hpp"
#include "IClock.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

DatagramDrops::DatagramDrops(const engine::IClock& clock)
    : clock_(&clock),
      dropLog_(std::string{SERVER_LOGGER_NAME},
               network::REJECTION_LINE_INTERVAL) {}

void DatagramDrops::drop(const network::Endpoint& sender,
                         std::string_view reason) {
  droppedCount_.fetch_add(1, std::memory_order_relaxed);
  logDrop(sender, reason);
}

void DatagramDrops::dropWrongToken(const network::Endpoint& sender) {
  logDrop(sender, WRONG_TOKEN_DROP_REASON);
  const std::scoped_lock lock{wrongTokenSenderMutex_};
  if (!wrongTokenSender_.has_value()) {
    wrongTokenSender_ = sender;
  }
  wrongTokenCount_.fetch_add(1, std::memory_order_relaxed);
}

std::size_t DatagramDrops::droppedCount() const {
  return droppedCount_.load(std::memory_order_relaxed);
}

std::size_t DatagramDrops::wrongTokenCount() const {
  return wrongTokenCount_.load(std::memory_order_relaxed);
}

std::optional<network::Endpoint> DatagramDrops::takeWrongTokenSender() {
  const std::scoped_lock lock{wrongTokenSenderMutex_};
  return std::exchange(wrongTokenSender_, std::nullopt);
}

void DatagramDrops::logDrop(const network::Endpoint& sender,
                            std::string_view reason) {
  dropLog_.debug(clock_->now(), "dropped a datagram from ", sender.address,
                 " port ", sender.port, ": ", reason);
}

}  // namespace rtype::server
