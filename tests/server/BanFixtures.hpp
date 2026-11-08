#pragma once

#include <chrono>
#include <optional>
#include <string_view>
#include "Ban.hpp"
#include "BanKind.hpp"

namespace rtype::server::testing {

/** @returns A ban of @p text as @p kind, from BAN_MOMENT until @p end. */
[[nodiscard]] Ban banOf(BanKind kind, std::string_view text,
                        std::optional<std::chrono::sys_seconds> end);

}  // namespace rtype::server::testing
