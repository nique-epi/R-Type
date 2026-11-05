#pragma once

#include <filesystem>
#include <string>

/**
 * @returns The file of the test plugin called @p name, in the folder the
 * tests build the plugins in: "<folder>/<name>.so" or "<folder>/<name>.dll".
 */
std::filesystem::path pluginFile(const std::string& name);

/**
 * @returns The folder the tests build the plugins in, as the text a
 * `plugin-folder` line takes: '/' between folders on every system.
 */
std::string pluginFolderText();
