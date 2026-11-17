#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace rtype::logging {

/**
 * @returns The value of the environment variable @p name, or nothing when it
 * is unset. On Windows it reads it with `_dupenv_s`, which the C runtime does
 * not deprecate as it does `getenv`.
 */
[[nodiscard]] std::optional<std::string> readEnvironment(std::string_view name);

}  // namespace rtype::logging
