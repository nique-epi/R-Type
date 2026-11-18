#pragma once

#include "ILayerCanvas.hpp"
#include "ILayerContent.hpp"
#include "ImageContent.hpp"
#include "LayerFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief A layer that shows one image, where its description puts it. In a
 * layer that repeats, the image shows again every @p repeat columns: a copy
 * that left on the left is followed by one that comes from the right.
 */
class ImageLayer final : public ILayerContent {
 public:
  /** @param repeat The width after which the layer repeats, 0 for never. */
  ImageLayer(ImageContent image, float repeat);

  void advance(engine::Duration elapsed, const LayerFrame& frame) override;
  void paint(ILayerCanvas& canvas, const LayerFrame& frame) const override;
  [[nodiscard]] int leftEdge() const override;

 private:
  ImageContent image_;
  int repeat_;
};

}  // namespace rtype::client
