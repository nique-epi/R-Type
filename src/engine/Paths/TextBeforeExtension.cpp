#include "TextBeforeExtension.hpp"
#include <filesystem>
#include <string_view>

namespace rtype::engine {

std::filesystem::path withTextBeforeExtension(const std::filesystem::path& file,
                                              std::string_view text) {
  std::filesystem::path name = file.stem();
  name += text;
  name += file.extension();
  return file.parent_path() / name;
}

}  // namespace rtype::engine
