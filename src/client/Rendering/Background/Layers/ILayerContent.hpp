#pragma once

#include "ILayerCanvas.hpp"
#include "LayerFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What a layer of a background shows: an image, the maps of an LDtk
 * layer or particles. It knows nothing of how its layer moves: it is told
 * where the layer stands (see LayerFrame).
 */
class ILayerContent {
 public:
  virtual ~ILayerContent() = default;

  /** @brief Moves what the content shows on by @p elapsed, a time above
   * zero, the layer standing at @p frame. */
  virtual void advance(engine::Duration elapsed, const LayerFrame& frame) = 0;

  /** @brief Paints the content on @p canvas, the layer standing at
   * @p frame. */
  virtual void paint(ILayerCanvas& canvas, const LayerFrame& frame) const = 0;

  /** @returns The column, counted from the origin of the layer, where what
   * the content shows begins: where a background that arrives through this
   * layer starts to show. */
  [[nodiscard]] virtual int leftEdge() const = 0;
};

}  // namespace rtype::client
