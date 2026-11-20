#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace rtype::client {

/** @brief Gives the waiting room the preview image of each map it shows. */
class IMapPreviews {
 public:
  virtual ~IMapPreviews() = default;

  /**
   * @returns The asset id of the loaded texture of the preview at @p path in
   * the pack of the server of the session; nothing when the session brings no
   * such file or it cannot be loaded.
   */
  [[nodiscard]] virtual std::optional<std::string> previewOf(
      std::string_view path) = 0;
};

}  // namespace rtype::client
