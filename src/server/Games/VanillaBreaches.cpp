#include "VanillaBreaches.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include "ServerConstants.hpp"

namespace rtype::server {

std::vector<std::string_view> vanillaBreaches(std::uint64_t wavesFingerprint,
                                              std::optional<std::uint32_t> seed,
                                              std::size_t inputDelayTicks) {
  std::vector<std::string_view> breaches;
  if (wavesFingerprint != OFFICIAL_WAVES_FINGERPRINT) {
    breaches.push_back(MODIFIED_WAVES_BREACH);
  }
  if (seed.has_value()) {
    breaches.push_back(IMPOSED_SEED_BREACH);
  }
  if (inputDelayTicks != DEFAULT_INPUT_DELAY_TICKS) {
    breaches.push_back(CHANGED_INPUT_DELAY_BREACH);
  }
  return breaches;
}

}  // namespace rtype::server
