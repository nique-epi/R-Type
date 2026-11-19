#include "RecordingMapPreviews.hpp"
#include <optional>
#include <string>
#include <string_view>

std::optional<std::string> RecordingMapPreviews::previewOf(
    std::string_view path) {
  if (path.empty()) {
    return std::nullopt;
  }
  return std::string{path};
}
