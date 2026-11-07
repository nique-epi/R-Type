#include "ByteText.hpp"
#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>

namespace rtype::network {

std::string textOf(std::span<const std::byte> bytes) {
  std::string text(bytes.size(), '\0');
  std::ranges::transform(bytes, text.begin(), [](std::byte value) {
    return static_cast<char>(value);
  });
  return text;
}

std::span<const std::byte> bytesOf(std::string_view text) {
  return std::as_bytes(std::span{text.data(), text.size()});
}

}  // namespace rtype::network
