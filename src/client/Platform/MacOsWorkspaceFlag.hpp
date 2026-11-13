#pragma once

namespace rtype::client {

/**
 * @brief Whether the shared NSWorkspace answers YES to the boolean property
 * named @p selector; false when AppKit or the property is missing. Call it
 * from the main thread, as AppKit requires. Defined on macOS only.
 */
[[nodiscard]] bool macOsWorkspaceFlag(const char* selector);

}  // namespace rtype::client
