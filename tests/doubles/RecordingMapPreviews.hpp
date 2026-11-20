#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "IMapPreviews.hpp"

/**
 * @brief Preview source double that loads nothing: it names each preview by
 * its path, as if every file of the pack were loaded, and nothing for an
 * empty path, a map without preview.
 */
class RecordingMapPreviews : public rtype::client::IMapPreviews {
 public:
  [[nodiscard]] std::optional<std::string> previewOf(
      std::string_view path) override;
};
