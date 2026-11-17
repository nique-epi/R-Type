#include "LayerCanvas.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <string_view>
#include <utility>
#include "FrameBatch.hpp"
#include "StripFrame.hpp"
#include "StripFrameRect.hpp"
#include "TextureLookup.hpp"
#include "TexturedQuad.hpp"

namespace rtype::client {

LayerCanvas::LayerCanvas(TextureLookup textureOf)
    : textureOf_(std::move(textureOf)) {}

void LayerCanvas::paintOn(sf::RenderTarget& target) {
  target_ = &target;
  painted_ = false;
  pixels_.clear();
  for (FrameBatch& batch : frames_) {
    batch.triangles.clear();
  }
}

void LayerCanvas::plot(sf::Vector2i pixel, sf::Color color) {
  pixels_.plot(pixel, color);
  painted_ = true;
}

void LayerCanvas::drawFrame(const StripFrame& frame, sf::Vector2i topLeft) {
  const sf::Texture* const image = textureOf_(frame.file);
  if (image == nullptr) {
    return;
  }
  const sf::FloatRect source = stripFrameRect(image->getSize(), frame);
  appendQuad(batchOf(*image).triangles,
             sf::FloatRect{sf::Vector2f(topLeft), source.size}, source);
  painted_ = true;
}

void LayerCanvas::drawTiles(std::string_view tileset,
                            const sf::VertexArray& tiles, int column) {
  const sf::Texture* const image = textureOf_(tileset);
  if (image == nullptr || target_ == nullptr || tiles.getVertexCount() == 0) {
    return;
  }
  painted_ = true;
  sf::RenderStates states(image);
  states.transform.translate({static_cast<float>(column), 0.0F});
  target_->draw(tiles, states);
}

bool LayerCanvas::flush() {
  if (target_ == nullptr || !painted_) {
    return false;
  }
  target_->draw(pixels_);
  for (const FrameBatch& batch : frames_) {
    if (batch.triangles.getVertexCount() > 0) {
      target_->draw(batch.triangles, sf::RenderStates(batch.image));
    }
  }
  return true;
}

FrameBatch& LayerCanvas::batchOf(const sf::Texture& image) {
  const auto found = std::ranges::find(frames_, &image, &FrameBatch::image);
  if (found != frames_.end()) {
    return *found;
  }
  frames_.push_back(FrameBatch{.image = &image});
  return frames_.back();
}

}  // namespace rtype::client
