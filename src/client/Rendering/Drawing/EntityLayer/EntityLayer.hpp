#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include "AssetTextureSource.hpp"
#include "ComponentRegistry.hpp"
#include "IEntityLayer.hpp"
#include "PixelBatch.hpp"
#include "PixelCanvas.hpp"
#include "RenderSystem.hpp"
#include "SfmlDrawSurface.hpp"
#include "TextWriter.hpp"

namespace rtype::client {

class AssetLibrary;

/**
 * @brief The entities of a game, painted on their own PixelCanvas.
 *
 * Their pixels are the pixels of the scrolling background: each sprite is
 * copied pixel for pixel at its native size, on whole background pixels, and
 * the canvas is enlarged like the sky. Each paint lays the pixel effects
 * first (see paintGroundEffects), then the sprites and the lines of text,
 * layer by layer (see RenderSystem), all shifted by the offset it is given.
 * Before the first paint the picture is empty, so laying it shows nothing.
 * The textures and the fonts come from an asset library, which must outlive
 * the layer and hold the fonts loaded.
 */
class EntityLayer : public IEntityLayer {
 public:
  /**
   * @throws RenderTextureNotCreatedException when the canvas cannot get its
   * render texture.
   * @throws AssetNotLoadedException when a font of the interface is not
   * loaded.
   */
  explicit EntityLayer(const AssetLibrary& library);

  EntityLayer(const EntityLayer&) = delete;
  EntityLayer& operator=(const EntityLayer&) = delete;
  EntityLayer(EntityLayer&&) = delete;
  EntityLayer& operator=(EntityLayer&&) = delete;
  ~EntityLayer() override = default;

  void paint(engine::ComponentRegistry& components,
             sf::Vector2i offset) override;
  void show(sf::RenderTarget& target) const override;

 private:
  AssetTextureSource textures_;
  TextWriter writer_;
  PixelCanvas canvas_;
  SfmlDrawSurface surface_;
  RenderSystem renderSystem_;
  PixelBatch groundPixels_;
};

}  // namespace rtype::client
