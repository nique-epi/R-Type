#pragma once

#include <string_view>

namespace sf {
class Texture;
}  // namespace sf

namespace rtype::client {

/**
 * @brief Finds the texture an asset id names.
 */
class ITextureSource {
 public:
  virtual ~ITextureSource() = default;

  /**
   * @returns The texture, or nullptr when the id names no loaded texture. The
   * pointer stays valid as long as the source keeps the texture loaded.
   */
  [[nodiscard]] virtual const sf::Texture* texture(
      std::string_view assetId) const = 0;
};

}  // namespace rtype::client
