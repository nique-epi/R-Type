#pragma once

#include <filesystem>
#include <optional>
#include "InstallPlace.hpp"
#include "LaunchCommand.hpp"

namespace rtype::client {

/**
 * @brief Puts @p package in place of the client installed at @p place, as far
 * as a running program may: Linux and macOS replace it now, Windows leaves it
 * to the installer the command starts.
 * @returns The command that starts the new version; nothing when a step
 * failed, the installed client then left as it was.
 */
[[nodiscard]] std::optional<LaunchCommand> installPackage(
    const std::filesystem::path& package, const InstallPlace& place);

/** @brief Removes what an earlier update left beside the installed client. */
void removeLeftovers(const InstallPlace& place);

}  // namespace rtype::client
