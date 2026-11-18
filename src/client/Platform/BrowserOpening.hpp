#pragma once

#include <string>

namespace rtype::client {

/**
 * @brief Asks the system to open @p url in the browser the player chose,
 * without waiting for it: `open` on macOS, `xdg-open` on Linux, both started
 * without a shell, and ShellExecuteW on Windows.
 * @returns Whether the system was asked: false for an address that starts
 * with neither HTTPS_URL_START nor HTTP_URL_START, which is never handed to
 * the system, and when the system refused to start the opener.
 */
[[nodiscard]] bool openInBrowser(const std::string& url);

/** @returns Whether @p url starts with HTTPS_URL_START or HTTP_URL_START, the
 * only addresses openInBrowser() hands to the system. */
[[nodiscard]] bool isWebAddress(const std::string& url);

}  // namespace rtype::client
