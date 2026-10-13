#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include "AssetTextureSource.hpp"
#include "ComponentRegistry.hpp"
#include "IEntityLayer.hpp"
#include "PixelCanvas.hpp"
#include "RenderSystem.hpp"
#include "SfmlDrawSurface.hpp"

namespace rtype::client {

class AssetLibrary;

/**
 * @brief The entities of a game, painted on their own PixelCanvas.
 *
 * Their pixels are the pixels of the scrolling background: each sprite is
 * copied pixel for pixel at its native size, on whole background pixels, and
 * the canvas is enlarged like the sky. Before the first paint the picture is
 * empty, so laying it shows nothing. The textures come from an asset library,
 * which must outlive the layer.
 */
class EntityLayer : public IEntityLayer {
 public:
  /**
   * @throws RenderTextureNotCreatedException when the canvas cannot get its
   * render texture.
   */
  explicit EntityLayer(const AssetLibrary& library);

  EntityLayer(const EntityLayer&) = delete;
  EntityLayer& operator=(const EntityLayer&) = delete;
  EntityLayer(EntityLayer&&) = delete;
  EntityLayer& operator=(EntityLayer&&) = delete;
  ~EntityLayer() override = default;

  void paint(engine::ComponentRegistry& components) override;
  void show(sf::RenderTarget& target) const override;

 private:
  AssetTextureSource textures_;
  PixelCanvas canvas_;
  SfmlDrawSurface surface_;
  RenderSystem renderSystem_;
};

}  // namespace rtype::client
