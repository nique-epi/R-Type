#include "BenchContentFolders.hpp"
#include <filesystem>
#include <string_view>
#include <vector>
#include "ScriptBudgetBenchConstants.hpp"

namespace rtype::tools {

std::vector<std::filesystem::path> benchContentFolders() {
  std::vector<std::filesystem::path> folders;
  folders.reserve(BENCH_CONTENT_FOLDERS.size());
  for (const std::string_view folder : BENCH_CONTENT_FOLDERS) {
    folders.push_back(std::filesystem::path{RTYPE_BENCH_REPOSITORY_ROOT} /
                      folder);
  }
  return folders;
}

}  // namespace rtype::tools
