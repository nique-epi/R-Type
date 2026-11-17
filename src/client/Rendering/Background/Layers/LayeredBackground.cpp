#include "LayeredBackground.hpp"
#include <chrono>
#include <cmath>
#include <cstddef>
#include <memory>
#include <optional>
#include "BackgroundConstants.hpp"
#include "BackgroundDescription.hpp"
#include "BackgroundPace.hpp"
#include "EmissionGroup.hpp"
#include "ILayerCanvas.hpp"
#include "ILayerContent.hpp"
#include "LayerBuilding.hpp"
#include "LayerDescription.hpp"
#include "LayerFrame.hpp"
#include "LayerMotion.hpp"
#include "LayerSources.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How many pixels of streak a particle of a layer that moves as
 * @p motion says leaves at @p speedFactor: what the layer covers in one
 * SIMULATION_TICK_DURATION, none up to STREAK_SPEED_FACTOR or for a layer
 * that does not warp. */
int streakOf(const LayerMotion& motion, float speedFactor) {
  if (!motion.warps || speedFactor <= STREAK_SPEED_FACTOR) {
    return 0;
  }
  const float tickSeconds =
      std::chrono::duration<float>(engine::SIMULATION_TICK_DURATION).count();
  return static_cast<int>(
      std::lround(motion.autoscroll * speedFactor * tickSeconds));
}

}  // namespace

LayeredBackground::LayeredBackground(const BackgroundDescription& description,
                                     const LayerSources& sources)
    : description_(&description),
      layers_(layersOf(description, sources)),
      drifted_(description.layers.size()) {}

void LayeredBackground::advance(engine::Duration elapsed,
                                const BackgroundPace& pace) {
  pace_ = pace;
  if (elapsed <= engine::Duration::zero()) {
    return;
  }
  const double seconds = std::chrono::duration<double>(elapsed).count();
  for (std::size_t layer = 0; layer < drifted_.size(); ++layer) {
    const LayerMotion& motion = description_->layers[layer].motion;
    const double factor = motion.warps ? pace.speedFactor : 1.0;
    drifted_[layer] += motion.autoscroll * factor * seconds;
  }
  for (EmissionGroup& group : layers_.groups) {
    group.advance(elapsed);
  }
  for (std::size_t layer = 0; layer < layers_.contents.size(); ++layer) {
    layers_.contents[layer]->advance(elapsed, frameOf(layer));
  }
}

bool LayeredBackground::shows(const BackgroundDescription& description) const {
  return description_ == &description;
}

std::size_t LayeredBackground::layerCount() const {
  return layers_.contents.size();
}

LayerFrame LayeredBackground::frameOf(std::size_t layer) const {
  const LayerDescription& described = description_->layers.at(layer);
  const int start = described.entrance ? static_cast<int>(BACKGROUND_WIDTH) : 0;
  return LayerFrame{
      .column = start - static_cast<int>(std::floor(movedBy(layer))),
      .streak = streakOf(described.motion, pace_.speedFactor)};
}

float LayeredBackground::shiftOf(std::size_t layer) const {
  if (description_->layers.at(layer).motion.parallax >= 1.0F) {
    return 0.0F;
  }
  const double moved = movedBy(layer);
  return -static_cast<float>(moved - std::floor(moved));
}

void LayeredBackground::paint(std::size_t layer, ILayerCanvas& canvas) const {
  layers_.contents.at(layer)->paint(canvas, frameOf(layer));
}

std::optional<std::size_t> LayeredBackground::entranceLayer() const {
  for (std::size_t layer = 0; layer < description_->layers.size(); ++layer) {
    if (description_->layers[layer].entrance) {
      return layer;
    }
  }
  return std::nullopt;
}

std::optional<int> LayeredBackground::entranceColumn() const {
  const std::optional<std::size_t> entrance = entranceLayer();
  if (!entrance.has_value()) {
    return std::nullopt;
  }
  return frameOf(*entrance).column + layers_.contents[*entrance]->leftEdge();
}

double LayeredBackground::movedBy(std::size_t layer) const {
  const LayerMotion& motion = description_->layers.at(layer).motion;
  return (static_cast<double>(pace_.scrolled) * motion.parallax) +
         drifted_.at(layer);
}

}  // namespace rtype::client
