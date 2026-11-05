#pragma once

#include <filesystem>
#include <string>

namespace rtype::logging {

/** @brief The UTF-8 text of a path, with '/' between folders on every
 * platform. */
[[nodiscard]] std::string genericText(const std::filesystem::path& path);

}  // namespace rtype::logging
