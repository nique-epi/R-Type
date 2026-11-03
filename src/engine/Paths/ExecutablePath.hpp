#pragma once

#include <filesystem>
#include <optional>

namespace rtype::engine {

/** @brief Path of the running executable, or nothing when the system does not
 * tell. */
std::optional<std::filesystem::path> executablePath();

/**
 * @brief Folder of the running executable, whatever folder the program was
 * started from.
 * @throws UnknownExecutablePathException when the system does not tell where
 * the executable is.
 */
std::filesystem::path executableFolder();

}  // namespace rtype::engine
