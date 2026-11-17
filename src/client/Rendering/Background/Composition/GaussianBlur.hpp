#pragma once

#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "Logger.hpp"

namespace rtype::client {

/**
 * @brief Blurs a background-sized texture on the graphics card, horizontally
 * then vertically, with a Gaussian of standard deviation
 * HALO_STANDARD_DEVIATION.
 *
 * Shaders may be missing on the machine, or fail to compile on its driver:
 * the blur is then unavailable, and the reason is logged once.
 */
class GaussianBlur {
 public:
  /**
   * @throws RenderTextureNotCreatedException when a pass cannot get its render
   * texture.
   */
  GaussianBlur();

  [[nodiscard]] bool isAvailable() const;

  /**
   * @brief Blurs a texture of BACKGROUND_SIZE. Only meaningful when available.
   *
   * @returns The blurred image, smoothed when stretched. It stays valid until
   * the next call.
   */
  [[nodiscard]] const sf::Texture& apply(const sf::Texture& source);

 private:
  static void createPass(sf::RenderTexture& pass);
  void blurInto(sf::RenderTexture& pass, const sf::Texture& source,
                sf::Glsl::Vec2 texelStep);

  logging::Logger logger_;
  sf::Shader shader_;
  sf::RenderTexture horizontalPass_;
  sf::RenderTexture verticalPass_;
  bool available_;
};

}  // namespace rtype::client
