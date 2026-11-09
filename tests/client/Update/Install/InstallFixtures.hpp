#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace rtype::client::testing {

/** @returns @p name in the temporary folder of the system, emptied and made
 * again. */
[[nodiscard]] std::filesystem::path emptyFolder(std::string_view name);

/** @brief Writes @p content to @p file. */
void writeFile(const std::filesystem::path& file, std::string_view content);

/** @returns What @p file holds. */
[[nodiscard]] std::string contentOf(const std::filesystem::path& file);

}  // namespace rtype::client::testing
