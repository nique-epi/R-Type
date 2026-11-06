#include "CapturedDatagrams.hpp"
#include <cstddef>
#include <span>
#include <vector>
#include "Endpoint.hpp"
#include "IDatagramSocket.hpp"

namespace rtype::network::testing {

DatagramHandler CapturedDatagrams::handler() {
  return
      [this](const Endpoint& /*sender*/, std::span<const std::byte> payload) {
        datagrams_.emplace_back(payload.begin(), payload.end());
      };
}

const std::vector<std::vector<std::byte>>& CapturedDatagrams::all() const {
  return datagrams_;
}

}  // namespace rtype::network::testing
