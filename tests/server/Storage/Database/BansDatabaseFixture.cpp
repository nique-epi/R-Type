#include "BansDatabaseFixture.hpp"
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include "Ban.hpp"
#include "BanKind.hpp"
#include "BanTarget.hpp"
#include "BanTestConstants.hpp"
#include "BansDatabaseTestConstants.hpp"

namespace rtype::server::testing {

Ban banOf(BanKind kind, std::string_view text,
          std::optional<std::chrono::sys_seconds> end) {
  return Ban{.target = BanTarget{.kind = kind, .text = std::string{text}},
             .bannedAt = BAN_MOMENT,
             .expiresAt = end};
}

std::filesystem::path BansDatabaseFixture::file() const {
  return folder_.path() / BANS_FILE_NAME;
}

}  // namespace rtype::server::testing
