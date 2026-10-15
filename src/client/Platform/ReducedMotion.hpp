#pragma once

namespace rtype::client {

/**
 * @brief Whether the player asked the system for less motion on screen.
 *
 * Reads the setting at each call: "Reduce motion" on macOS, "Show animations
 * in Windows" turned off on Windows, the GNOME setting enable-animations
 * turned off on Linux. A setting the system does not give counts as no
 * request. On macOS, call it from the main thread, as AppKit requires.
 */
[[nodiscard]] bool systemPrefersReducedMotion();

}  // namespace rtype::client
