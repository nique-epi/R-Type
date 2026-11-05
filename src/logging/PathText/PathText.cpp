#include "PathText.hpp"
#include <filesystem>
#include <string>

namespace rtype::logging {

std::string genericText(const std::filesystem::path& path) {
  std::string text;
  for (const char8_t character : path.generic_u8string()) {
    text.push_back(static_cast<char>(character));
  }
  return text;
}

}  // namespace rtype::logging
