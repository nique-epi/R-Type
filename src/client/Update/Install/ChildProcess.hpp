#pragma once

#include "LaunchCommand.hpp"

namespace rtype::client {

/** @brief Starts @p command and does not wait for it: the new program goes on
 * once this client closed. On Windows the system asks the player to allow it
 * when the program needs it, as an installer does.
 * @returns Whether it started. */
[[nodiscard]] bool launchDetached(const LaunchCommand& command);

/** @brief Runs @p command to its end; macOS and Linux only, Windows does not
 * define it. @returns Its exit status, or LAUNCH_FAILED_STATUS when it did not
 * start or did not exit by itself. */
[[nodiscard]] int runToEnd(const LaunchCommand& command);

}  // namespace rtype::client
