#include "BanFixtures.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include "Ban.hpp"
#include "BanKind.hpp"
#include "BanTarget.hpp"
#include "BanTestConstants.hpp"

namespace rtype::server::testing {

Ban banOf(BanKind kind, std::string_view text,
          std::optional<std::chrono::sys_seconds> end) {
  return Ban{.target = BanTarget{.kind = kind, .text = std::string{text}},
             .bannedAt = BAN_MOMENT,
             .expiresAt = end};
}

}  // namespace rtype::server::testing
