#include "AssetIds.hpp"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>
#include "AssetConstants.hpp"

namespace rtype::client {

namespace {

bool isLowercaseLetterOrDigit(char character) {
  return (character >= 'a' && character <= 'z') ||
         (character >= '0' && character <= '9');
}

bool isAllowedInSegment(char character) {
  return isLowercaseLetterOrDigit(character) ||
         std::ranges::find(ASSET_ID_PUNCTUATION, character) !=
             ASSET_ID_PUNCTUATION.end();
}

bool isValidSegment(std::string_view segment) {
  return !segment.empty() && isLowercaseLetterOrDigit(segment.front()) &&
         std::ranges::all_of(segment, isAllowedInSegment);
}

}  // namespace

bool isValidAssetId(std::string_view assetId) {
  std::string_view remaining = assetId;
  while (true) {
    const std::size_t separator = remaining.find(ASSET_ID_SEPARATOR);
    if (!isValidSegment(remaining.substr(0, separator))) {
      return false;
    }
    if (separator == std::string_view::npos) {
      return true;
    }
    remaining.remove_prefix(separator + 1);
  }
}

std::string genericText(const std::filesystem::path& path) {
  std::string text;
  for (const char8_t character : path.generic_u8string()) {
    text.push_back(static_cast<char>(character));
  }
  return text;
}

}  // namespace rtype::client
