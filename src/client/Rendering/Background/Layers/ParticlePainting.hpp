#pragma once

#include "ILayerCanvas.hpp"
#include "PaintedParticle.hpp"
#include "ParticleContent.hpp"

namespace rtype::client {

/**
 * @brief Paints one particle of the layer @p description describes on
 * @p canvas: the streak it leaves in a warp, then its trail, each pixel in
 * the color of its share of the trail, then the particle, a pixel in its
 * color or in its twinkle color, or the frame its sprite shows now.
 */
void paintParticle(ILayerCanvas& canvas, const ParticleContent& description,
                   const PaintedParticle& painted);

}  // namespace rtype::client
