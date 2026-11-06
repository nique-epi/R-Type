#pragma once

#include <filesystem>
#include <string>

/**
 * @returns The file of the test plugin called @p name, in the folder of the
 * plugins the test executable loads: "<folder>/<name>.so" or
 * "<folder>/<name>.dll".
 */
std::filesystem::path pluginFile(const std::string& name);

/**
 * @returns The folder of the plugins the test executable loads, which its
 * target names in RTYPE_TEST_PLUGIN_FOLDER (the test plugins for the engine,
 * the plugins of the build for the server), as the text a `plugin-folder` line
 * takes: '/' between folders on every system.
 */
std::string pluginFolderText();
