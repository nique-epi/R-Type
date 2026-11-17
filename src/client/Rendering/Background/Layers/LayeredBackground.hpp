#pragma once

#include <cstddef>
#include <optional>
#include <vector>
#include "BackgroundDescription.hpp"
#include "BackgroundPace.hpp"
#include "BuiltLayers.hpp"
#include "ILayerCanvas.hpp"
#include "LayerFrame.hpp"
#include "LayerSources.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief One background on screen: what each of its layers shows (see
 * layersOf()), and how far each has moved.
 *
 * A layer moves to the left by its parallax times the scroll of the level,
 * plus its autoscroll times the time, the latter times the speed factor when
 * it warps. It is painted on whole pixels and laid shiftOf() away from them,
 * so a slow layer moves by less than a pixel; a layer that follows the whole
 * scroll keeps the grid of the playfield. The layer a background arrives
 * through starts at the right edge of the background instead of its left.
 *
 * It refers to its description, which must outlive it: the catalog keeps it.
 */
class LayeredBackground {
 public:
  LayeredBackground(const BackgroundDescription& description,
                    const LayerSources& sources);

  /** @brief Moves the layers to where @p pace puts them and, when
   * @p elapsed is above zero, on by that time: their own motion, the births
   * of the emission groups, then what each layer shows. */
  void advance(engine::Duration elapsed, const BackgroundPace& pace);

  /** @returns Whether this is the background @p description describes. */
  [[nodiscard]] bool shows(const BackgroundDescription& description) const;

  [[nodiscard]] std::size_t layerCount() const;

  /** @returns Where the layer @p layer stands. */
  [[nodiscard]] LayerFrame frameOf(std::size_t layer) const;

  /** @returns How far right of the pixels it is painted on the layer
   * @p layer lies, from -1 excluded to 0 included. */
  [[nodiscard]] float shiftOf(std::size_t layer) const;

  /** @brief Paints the layer @p layer on @p canvas. */
  void paint(std::size_t layer, ILayerCanvas& canvas) const;

  /** @returns The rank of the layer the background arrives through; none
   * for a background that shows whole at once. */
  [[nodiscard]] std::optional<std::size_t> entranceLayer() const;

  /** @returns The column of the background where its entrance begins, left
   * of which the layers behind the entrance do not show yet; none without an
   * entrance. */
  [[nodiscard]] std::optional<int> entranceColumn() const;

 private:
  /** @returns How many pixels the layer @p layer has moved to the left. */
  [[nodiscard]] double movedBy(std::size_t layer) const;

  const BackgroundDescription* description_;
  BuiltLayers layers_;
  std::vector<double> drifted_;
  BackgroundPace pace_;
};

}  // namespace rtype::client
