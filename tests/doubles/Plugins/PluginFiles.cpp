#include "PluginFiles.hpp"
#include <filesystem>
#include <string>
#include "PathText.hpp"
#include "SharedLibrary.hpp"

std::filesystem::path pluginFile(const std::string& name) {
  return std::filesystem::path{RTYPE_TEST_PLUGIN_FOLDER} /
         (name + std::string{rtype::engine::SharedLibrary::fileSuffix()});
}

std::string pluginFolderText() {
  return rtype::engine::genericText(
      std::filesystem::path{RTYPE_TEST_PLUGIN_FOLDER});
}
