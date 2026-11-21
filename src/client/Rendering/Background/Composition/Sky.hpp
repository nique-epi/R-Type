#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include "BackgroundCatalog.hpp"
#include "GaussianBlur.hpp"
#include "ISky.hpp"
#include "LayerCanvas.hpp"
#include "LayeredBackground.hpp"
#include "SeamCover.hpp"
#include "ShownBackgrounds.hpp"
#include "SkyLevel.hpp"
#include "SkyPicture.hpp"
#include "SkySources.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Draws the background behind the playfield: the layers of the
 * background shown (see ShownBackgrounds), from the farthest to the nearest,
 * and a blurred copy of them added on top as a halo when shaders are
 * available.
 *
 * Each layer is painted apart on a plane of a SkyPicture, so a slow layer
 * moves by less than a pixel while the pixels stay square, equal and sharp at
 * every window size. While a background arrives, the one that leaves is
 * painted first, then the one that arrives, whose layers behind its entrance
 * only show right of where the entrance begins (see SeamCover). The picture
 * is filled with SKY_COLOR before the planes are laid, so a shift never
 * uncovers the window behind.
 */
class Sky final : public ISky {
 public:
  /**
   * @param catalog The backgrounds the sky may show; it must outlive the
   * sky.
   * @throws RenderTextureNotCreatedException when the sky or the halo cannot
   * get a render texture.
   */
  Sky(const BackgroundCatalog& catalog, SkySources sources);

  /** @brief Moves the backgrounds, see ShownBackgrounds::advance(). */
  void advance(engine::Duration elapsed);

  void setSpeedFactor(float factor) override;
  void setOffset(sf::Vector2i pixels) override;
  void showLevel(const SkyLevel& level) override;
  [[nodiscard]] std::optional<std::string> biomeMusic() const override;

  /** @brief Lays the backgrounds of the pack of a server over those of the
   * sky, or takes them off for nullptr, see ShownBackgrounds::layOver(). */
  void layOver(std::shared_ptr<const BackgroundCatalog> backgrounds);

  /**
   * @brief Draws the sky over the whole playfield.
   *
   * @param target Target whose view shows the playfield in its logical units.
   * @throws RenderTextureNotCreatedException when the sky cannot get a
   * render texture of the size it needs.
   */
  void draw(sf::RenderTarget& target);

 private:
  /** @brief Paints each layer of @p background on its plane of the picture,
   * from the plane @p firstPlane on. */
  void paintLayers(const LayeredBackground& background, std::size_t firstPlane);

  /** @brief Adds the blurred sky on top of @p target, when shaders are
   * available. */
  void drawHalo(sf::RenderTarget& target);

  ShownBackgrounds backgrounds_;
  const BackgroundCatalog* catalog_;
  std::shared_ptr<const BackgroundCatalog> laidOver_;
  std::string biome_;
  LayerCanvas canvas_;
  SkyPicture picture_;
  SeamCover entranceMask_;
  GaussianBlur halo_;
  sf::Vector2i offset_;
};

}  // namespace rtype::client
