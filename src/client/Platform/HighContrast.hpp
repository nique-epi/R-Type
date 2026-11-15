#pragma once

namespace rtype::client {

/**
 * @brief Whether the player asked the system for more contrast on screen.
 *
 * Reads the setting at each call: "Increase contrast" on macOS, a high
 * contrast theme on Windows, the contrast of the freedesktop Settings portal
 * on Linux. A setting the system does not give counts as no request. On
 * macOS, call it from the main thread, as AppKit requires.
 */
[[nodiscard]] bool systemPrefersHighContrast();

}  // namespace rtype::client
