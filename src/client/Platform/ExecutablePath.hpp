#pragma once

#include <filesystem>
#include <optional>

namespace rtype::client {

/** @brief Path of the running executable, or nothing when the system does not
 * tell. */
std::optional<std::filesystem::path> executablePath();

}  // namespace rtype::client
