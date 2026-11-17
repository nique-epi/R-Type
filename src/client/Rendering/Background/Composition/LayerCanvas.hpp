#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>
#include <vector>
#include "FrameBatch.hpp"
#include "ILayerCanvas.hpp"
#include "PixelBatch.hpp"
#include "StripFrame.hpp"
#include "TextureLookup.hpp"

namespace rtype::client {

/**
 * @brief Paints a layer of a background on a render target whose view shows
 * pixels of the background.
 *
 * The pixels and the frames a layer asks for are collected, then drawn by
 * flush(): the pixels in one call, then the frames of each image in one call,
 * so a layer of a thousand stars costs as little as one. Tiles are drawn as
 * they are asked for. An image that is not loaded draws nothing.
 */
class LayerCanvas final : public ILayerCanvas {
 public:
  /** @param textureOf Finds the image an asset id names; it must not be
   * empty. */
  explicit LayerCanvas(TextureLookup textureOf);

  /** @brief From now on, paints on @p target, which must stay alive until
   * flush(); what was collected before is forgotten. */
  void paintOn(sf::RenderTarget& target);

  void plot(sf::Vector2i pixel, sf::Color color) override;
  void drawFrame(const StripFrame& frame, sf::Vector2i topLeft) override;
  void drawTiles(std::string_view tileset, const sf::VertexArray& tiles,
                 int column) override;

  /** @brief Draws the pixels, then the frames, collected since paintOn(). */
  void flush();

 private:
  /** @returns The batch of the frames drawn from @p image. */
  [[nodiscard]] FrameBatch& batchOf(const sf::Texture& image);

  TextureLookup textureOf_;
  sf::RenderTarget* target_{};
  PixelBatch pixels_;
  std::vector<FrameBatch> frames_;
};

}  // namespace rtype::client
