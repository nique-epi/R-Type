#pragma once

#include <filesystem>
#include <string_view>

namespace rtype::client {

/** @brief The key of the package of each system in the manifest, as CPack
 * names the files: R-Type-client-<version>-<system>. */
constexpr std::string_view LINUX_PACKAGE_SYSTEM = "Linux";
constexpr std::string_view MACOS_PACKAGE_SYSTEM = "Darwin";
constexpr std::string_view WINDOWS_PACKAGE_SYSTEM = "win64";

/** @brief The variable the AppImage runtime sets to the path of the
 * AppImage. */
constexpr const char* APPIMAGE_VARIABLE = "APPIMAGE";
constexpr std::string_view PARTIAL_INSTALL_SUFFIX = ".part";
constexpr std::filesystem::perms APPIMAGE_PERMISSIONS =
    std::filesystem::perms::owner_all | std::filesystem::perms::group_read |
    std::filesystem::perms::group_exec | std::filesystem::perms::others_read |
    std::filesystem::perms::others_exec;

/** @brief The bundle of macOS: R-Type.app/Contents/MacOS/r-type_client. */
constexpr std::string_view APP_BUNDLE_NAME = "R-Type.app";
constexpr std::string_view APP_BUNDLE_EXTENSION = ".app";
constexpr std::string_view MACOS_EXECUTABLE_FOLDER = "MacOS";
/** @brief A folder of the path of an app macOS runs translocated, from a
 * read-only copy no update can replace. */
constexpr std::string_view TRANSLOCATION_FOLDER = "AppTranslocation";
constexpr std::string_view STAGED_BUNDLE_SUFFIX = ".new";
constexpr std::string_view REPLACED_BUNDLE_SUFFIX = ".old";
constexpr std::string_view DISK_IMAGE_TOOL = "/usr/bin/hdiutil";
constexpr std::string_view COPY_TOOL = "/usr/bin/ditto";
constexpr std::string_view OPEN_TOOL = "/usr/bin/open";
constexpr std::string_view DISK_IMAGE_MOUNT_FOLDER = "volume";
constexpr std::string_view OPEN_NEW_INSTANCE_OPTION = "-n";

/** @brief The installer CPack writes for NSIS, in the folder it installs to,
 * and the options of a silent install there. */
constexpr std::string_view WINDOWS_UNINSTALLER_NAME = "Uninstall.exe";
constexpr std::string_view SILENT_INSTALL_OPTION = "/S";
constexpr std::string_view INSTALL_FOLDER_OPTION = "/D=";

/** @brief Folder of the temporary folder of the system the package is written
 * to, emptied at each start. */
constexpr std::string_view DOWNLOAD_FOLDER_NAME = "R-Type-update";

/** @brief File of the settings folder that keeps the notes of the release
 * installed, until WHAT'S NEW shows them, and its members. */
constexpr std::string_view RELEASE_NOTES_FILE_NAME = "update-notes.json";
constexpr std::string_view NOTES_VERSION_KEY = "version";
constexpr std::string_view NOTES_LINES_KEY = "notes";

/** @brief What runToEnd() returns when the program could not be started or
 * did not exit by itself. */
constexpr int LAUNCH_FAILED_STATUS = -1;

}  // namespace rtype::client
