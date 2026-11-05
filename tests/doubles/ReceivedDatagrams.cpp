#include "ReceivedDatagrams.hpp"
#include <cstddef>
#include <span>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"

namespace rtype::network::testing {

DatagramHandler ReceivedDatagrams::handler() {
  return
      [this](const Endpoint& /*sender*/, std::span<const std::byte> payload) {
        datagrams_.emplace_back(payload.begin(), payload.end());
      };
}

const std::vector<std::vector<std::byte>>& ReceivedDatagrams::all() const {
  return datagrams_;
}

}  // namespace rtype::network::testing
