#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::client {

constexpr const char* LINUX_EXECUTABLE_LINK = "/proc/self/exe";

/** @brief Longest path Windows returns for a module, in characters. */
constexpr std::size_t MAXIMUM_WINDOWS_PATH_LENGTH = 32768;

/**
 * @brief Objective-C names of the macOS property that tells whether "Reduce
 * motion" is on: accessibilityDisplayShouldReduceMotion of the shared
 * NSWorkspace, from macOS 10.12.
 */
constexpr const char* MACOS_WORKSPACE_CLASS = "NSWorkspace";
constexpr const char* MACOS_SHARED_WORKSPACE_SELECTOR = "sharedWorkspace";
constexpr const char* MACOS_REDUCE_MOTION_SELECTOR =
    "accessibilityDisplayShouldReduceMotion";

/**
 * @brief Command that prints the GNOME setting enable-animations, "true" or
 * "false", its errors thrown away; what it prints when animations are off;
 * and the longest answer read, in characters.
 */
constexpr const char* GNOME_ANIMATIONS_COMMAND =
    "gsettings get org.gnome.desktop.interface enable-animations 2>/dev/null";
constexpr std::string_view GNOME_ANIMATIONS_OFF = "false";
constexpr std::size_t GNOME_ANSWER_LENGTH = 16;

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

}  // namespace rtype::client
