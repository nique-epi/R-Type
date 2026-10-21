#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include "AssetTextureSource.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "Logger.hpp"
#include "PixelCanvas.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

class AssetLibrary;

/**
 * @brief An interface picture painted on its own PixelCanvas, so its pixels
 * are the pixels of the sky: square, equal and sharp at every window size.
 *
 * Text is written with the two pixel fonts at TEXT_CHARACTER_SIZE, the size
 * they are designed for, on whole pixels. A line is placed as the browser
 * places a line as tall as its font: its baseline sits
 * REGULAR_FONT_BASELINE_ROW or SMALL_FONT_BASELINE_ROW below the anchor, while
 * SFML puts the baseline of a text TEXT_CHARACTER_SIZE below its position. An
 * outlined line is first drawn in INK one pixel away in each of the eight
 * directions of CROSS_DIRECTIONS and DIAGONAL_DIRECTIONS. Text is drawn
 * through CRISP_TEXT_FRAGMENT_SHADER, so its pixels are either lit or
 * transparent; where shaders are missing or the shader does not compile, it
 * is drawn smoothed, and the reason is logged once.
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
  /** @brief A font of the interface, and the row of its baseline below the
   * top of a line. */
  struct LoadedFont {
    const sf::Font* font{};
    int baselineRow{};
  };

  /** @brief A texture laid smoothed over the picture: its smoothed copy, the
   * area it covers, in pixels of the picture, and its tint. */
  struct SmoothImage {
    const sf::Texture* texture{};
    sf::IntRect area;
    sf::Color tint;
  };

  [[nodiscard]] const LoadedFont& loadedFontOf(TextFont font) const;

  /** @returns A line of text in a font, at TEXT_CHARACTER_SIZE, not placed. */
  [[nodiscard]] sf::Text lineOf(std::string_view text, TextFont font) const;

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
  LoadedFont regularFont_;
  LoadedFont smallFont_;
  PixelCanvas canvas_;
  logging::Logger logger_;
  std::set<std::string, std::less<>> reportedMissingIds_;
  std::optional<sf::Shader> crispText_;
  std::map<std::string, sf::Texture, std::less<>> smoothTextures_;
  std::vector<SmoothImage> paintedSmoothImages_;
  std::vector<SmoothImage> shownSmoothImages_;
};

}  // namespace rtype::client
