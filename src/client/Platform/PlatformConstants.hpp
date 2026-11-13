#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::client {

/**
 * @brief Objective-C names of the macOS property that tells whether "Reduce
 * motion" is on: accessibilityDisplayShouldReduceMotion of the shared
 * NSWorkspace, from macOS 10.12.
 */
constexpr const char* MACOS_WORKSPACE_CLASS = "NSWorkspace";
constexpr const char* MACOS_SHARED_WORKSPACE_SELECTOR = "sharedWorkspace";
constexpr const char* MACOS_REDUCE_MOTION_SELECTOR =
    "accessibilityDisplayShouldReduceMotion";

/** @brief Objective-C name of the macOS property that tells whether
 * "Increase contrast" is on: accessibilityDisplayShouldIncreaseContrast of
 * the shared NSWorkspace, from macOS 10.10. */
constexpr const char* MACOS_INCREASE_CONTRAST_SELECTOR =
    "accessibilityDisplayShouldIncreaseContrast";

/**
 * @brief Command that prints the GNOME setting enable-animations, "true" or
 * "false", its errors thrown away; and what it prints when animations are
 * off.
 */
constexpr const char* GNOME_ANIMATIONS_COMMAND =
    "gsettings get org.gnome.desktop.interface enable-animations 2>/dev/null";
constexpr std::string_view GNOME_ANIMATIONS_OFF = "false";

/**
 * @brief Command that asks the freedesktop Settings portal for the contrast
 * of org.freedesktop.appearance, its errors thrown away, and what its answer,
 * "(<uint32 1>,)", holds when the system asks for a higher contrast. The
 * portal gives 0 for no preference and 1 for a higher contrast, and any other
 * value counts as 0.
 */
constexpr const char* PORTAL_CONTRAST_COMMAND =
    "gdbus call --session --dest org.freedesktop.portal.Desktop "
    "--object-path /org/freedesktop/portal/desktop "
    "--method org.freedesktop.portal.Settings.ReadOne "
    "org.freedesktop.appearance contrast 2>/dev/null";
constexpr std::string_view PORTAL_HIGHER_CONTRAST = "<uint32 1>";

/** @brief The longest first line read from what a command prints, in
 * characters. */
constexpr std::size_t COMMAND_ANSWER_LENGTH = 32;

/** @brief Mode that opens a command to read what it prints. */
constexpr const char* COMMAND_READ_MODE = "r";

/**
 * @brief Where the settings folder is found: the XDG variable of the
 * configuration folder and the home variable; the configuration folder of
 * the home on Linux; the name of the client's folder on each system; and the
 * character that stands for the home in the paths sysdir gives on macOS.
 */
constexpr const char* XDG_CONFIG_HOME_VARIABLE = "XDG_CONFIG_HOME";
constexpr const char* HOME_VARIABLE = "HOME";
constexpr const char* LINUX_CONFIGURATION_FOLDER = ".config";
constexpr const char* LINUX_SETTINGS_FOLDER_NAME = "r-type";
constexpr const char* MACOS_SETTINGS_FOLDER_NAME = "R-Type";
constexpr const wchar_t* WINDOWS_SETTINGS_FOLDER_NAME = L"R-Type";
constexpr char HOME_SHORTHAND = '~';

/** @brief Folder of the user's Library where macOS keeps the logs of the
 * user's programs. */
constexpr const char* MACOS_LOGS_FOLDER_NAME = "Logs";

}  // namespace rtype::client
