#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

namespace rtype::client {

/** @brief A version of the game, `<major>.<minor>.<patch>` as project()
 * writes it, ordered number by number: 1.10.0 is newer than 1.9.9. The
 * members are not named major and minor, which glibc defines as macros. */
struct ReleaseVersion {
  std::uint32_t majorNumber{};
  std::uint32_t minorNumber{};
  std::uint32_t patchNumber{};

  auto operator<=>(const ReleaseVersion& other) const = default;
};

/** @returns The version @p text writes, or nothing when it is not exactly
 * three whole numbers of std::uint32_t joined by dots. */
[[nodiscard]] std::optional<ReleaseVersion> releaseVersionOf(
    std::string_view text);

}  // namespace rtype::client
