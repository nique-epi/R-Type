#pragma once

#include "ActiveHurts.hpp"
#include "ComponentRegistry.hpp"
#include "IPixelSurface.hpp"

namespace rtype::client {

/**
 * @brief Paints, in pixels of the canvas, every hurt beam of a PaintedBeams
 * that hurts in the action it shows, where the server lays it
 * (game::layActiveHurts(), with the shown action and its time): a band as
 * thick as the beam, its core white, then pale pink, pink and a deep magenta
 * rim towards its edges. A pixel is painted when its center lies on the band:
 * no farther than half the thickness from the line, between the pivot and
 * the end. It keeps its ActiveHurts between two frames, so it allocates
 * nothing once warm.
 */
class HurtBeamPainter {
 public:
  void paint(engine::ComponentRegistry& components, IPixelSurface& surface);

 private:
  game::ActiveHurts hurts_;
};

}  // namespace rtype::client
