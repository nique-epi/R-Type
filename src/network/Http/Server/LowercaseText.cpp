#include "LowercaseText.hpp"
#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

namespace rtype::network {

std::string lowercaseOf(std::string_view text) {
  std::string lowercase{text};
  std::ranges::transform(lowercase, lowercase.begin(), [](char letter) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
  });
  return lowercase;
}

}  // namespace rtype::network
