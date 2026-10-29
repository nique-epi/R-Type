#pragma once

namespace rtype::client {

/**
 * @brief Whether a borderless window must be SFML's fullscreen at the video
 * mode of the desktop, rather than a window without border as large as the
 * desktop.
 *
 * true on macOS and Linux, where a window without border stays under the menu
 * bar, the Dock or the panels; false on Windows, where the taskbar steps aside
 * for a window that covers the whole screen.
 */
[[nodiscard]] bool borderlessUsesDesktopFullscreen();

}  // namespace rtype::client
