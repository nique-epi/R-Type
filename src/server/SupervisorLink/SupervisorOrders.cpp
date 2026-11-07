#include "SupervisorOrders.hpp"
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include "DatagramCodec.hpp"
#include "ExpulsionOrder.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"

namespace rtype::server {

bool holdsStopOrder(std::span<const std::byte> datagram) {
  const std::optional<network::DatagramCodec::MessageTypes> types =
      network::DatagramCodec::messageTypes(datagram);
  return types.has_value() && types->test(static_cast<std::size_t>(
                                  network::MessageType::Disconnection));
}

std::optional<std::string> expulsionIn(std::span<const std::byte> datagram) {
  try {
    const std::optional<network::ExpulsionOrder> order =
        network::DatagramCodec::firstMessage<network::ExpulsionOrder>(datagram);
    if (!order.has_value()) {
      return std::nullopt;
    }
    return order->nickname;
  } catch (const network::NetworkException&) {
    return std::nullopt;
  }
}

}  // namespace rtype::server
