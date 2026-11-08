#include "BehaviourScriptFiles.hpp"
#include <algorithm>
#include <filesystem>
#include <span>
#include <system_error>
#include <vector>
#include "DataFileReading.hpp"
#include "GameScriptingConstants.hpp"
#include "PathText.hpp"
#include "ScriptSource.hpp"

namespace rtype::game {

namespace {

/** @returns The scripts right in @p folder, sorted by path. */
std::vector<std::filesystem::path> scriptFilesIn(
    const std::filesystem::path& folder) {
  std::vector<std::filesystem::path> files;
  std::error_code error;
  if (!std::filesystem::is_directory(folder, error)) {
    return files;
  }
  for (const auto& entry : std::filesystem::directory_iterator(folder, error)) {
    if (entry.is_regular_file(error) &&
        entry.path().extension() == SCRIPT_FILE_EXTENSION) {
      files.push_back(entry.path());
    }
  }
  std::ranges::sort(files);
  return files;
}

}  // namespace

std::vector<engine::ScriptSource> scriptSourcesIn(
    std::span<const std::filesystem::path> contentFolders) {
  std::vector<engine::ScriptSource> sources;
  for (const std::filesystem::path& folder : contentFolders) {
    const std::filesystem::path scripts = folder / SCRIPT_FOLDER_IN_CONTENT;
    for (const std::filesystem::path& file : scriptFilesIn(scripts)) {
      sources.push_back(engine::ScriptSource{
          .name = logging::genericText(
              folder.filename() / SCRIPT_FOLDER_IN_CONTENT / file.filename()),
          .text = engine::dataFileContent(file)});
    }
  }
  return sources;
}

}  // namespace rtype::game
