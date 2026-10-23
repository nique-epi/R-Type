#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include "AssetTextureSource.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "Logger.hpp"
#include "PixelCanvas.hpp"
#include "SmoothImage.hpp"
#include "TextStyle.hpp"
#include "TextWriter.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

class AssetLibrary;

/**
 * @brief An interface picture painted on its own PixelCanvas, so its pixels
 * are the pixels of the sky: square, equal and sharp at every window size.
 *
 * Text is written by a TextWriter: the two pixel fonts at
 * TEXT_CHARACTER_SIZE, on whole pixels, outlined in INK when asked, through
 * the crisp text shader (see TextWriter).
 *
 * A smooth image is not part of the pixel picture: display() keeps the list of
 * those drawn since clear(), and show() lays them over the picture, each
 * stretched over its area at the resolution of the window, from a copy of its
 * texture smoothed and given mipmaps, made the first time its id is drawn.
 *
 * Fonts and textures come from an asset library, which must outlive the
 * canvas: the fonts must be loaded when the canvas is built, a texture when it
 * is drawn. A texture that is not loaded is reported once in the log.
 */
class InterfaceCanvas : public IInterfaceCanvas {
 public:
  /**
   * @throws AssetNotLoadedException when a font of the interface is not
   * loaded.
   * @throws RenderTextureNotCreatedException when the canvas cannot get its
   * render texture.
   */
  explicit InterfaceCanvas(const AssetLibrary& library);

  InterfaceCanvas(const InterfaceCanvas&) = delete;
  InterfaceCanvas& operator=(const InterfaceCanvas&) = delete;
  InterfaceCanvas(InterfaceCanvas&&) = delete;
  InterfaceCanvas& operator=(InterfaceCanvas&&) = delete;
  ~InterfaceCanvas() override = default;

  void clear() override;
  void fill(sf::IntRect area, sf::Color color) override;
  void write(std::string_view text, sf::Vector2i anchor,
             TextStyle style) override;
  void drawImage(TextureArea image, ImagePlacement placement,
                 sf::IntRect visibleArea) override;
  void drawSmoothImage(std::string_view assetId, sf::IntRect area,
                       sf::Color tint) override;
  [[nodiscard]] int textWidth(std::string_view text,
                              TextFont font) const override;
  void display() override;
  void show(sf::RenderTarget& target) const override;

 private:
  /** @returns The loaded texture of an id, or nullptr once the log says it is
   * missing; a missing id is reported once. */
  [[nodiscard]] const sf::Texture* foundTexture(std::string_view assetId);

  /** @returns The smoothed copy of the texture of an id, made the first time
   * the id asks for it (see addSmoothCopy). */
  [[nodiscard]] const sf::Texture& smoothCopyOf(std::string_view assetId,
                                                const sf::Texture& texture);

  /** @returns A new copy of a texture for an id, smoothed and given mipmaps,
   * so drawing it smaller than its size stays smooth; without mipmaps, the
   * copy is smoothed only and the log says so. */
  [[nodiscard]] const sf::Texture& addSmoothCopy(std::string_view assetId,
                                                 const sf::Texture& texture);

  /** @brief Lays the textures of the last displayed picture over it, each
   * smoothed over its area, in logical units of the playfield. */
  void showSmoothImages(sf::RenderTarget& target) const;

  AssetTextureSource textures_;
  TextWriter writer_;
  PixelCanvas canvas_;
  logging::Logger logger_;
  std::set<std::string, std::less<>> reportedMissingIds_;
  std::map<std::string, sf::Texture, std::less<>> smoothTextures_;
  std::vector<SmoothImage> paintedSmoothImages_;
  std::vector<SmoothImage> shownSmoothImages_;
};

}  // namespace rtype::client
