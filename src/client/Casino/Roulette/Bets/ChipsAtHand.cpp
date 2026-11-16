#include "ChipsAtHand.hpp"
#include <cstdint>
#include <optional>
#include "BetsReceipt.hpp"

namespace rtype::client {

std::uint32_t chipsAtHandOf(
    std::uint32_t walletChips,
    const std::optional<network::BetsReceipt>& accepted) {
  if (accepted.has_value()) {
    return accepted->chips + accepted->stake;
  }
  return walletChips;
}

}  // namespace rtype::client
