#pragma once

#include <cstddef>
#include <string_view>
#include <vector>
#include "ColoredPixel.hpp"
#include "LayerFrame.hpp"
#include "ParticleLayer.hpp"
#include "RecordedFrame.hpp"

/** @returns A layer of the particles @p keys describes, as
 * particlesReading() takes them, that repeats every @p repeat columns. */
[[nodiscard]] rtype::client::ParticleLayer particleLayerOf(
    std::string_view keys, float repeat);

/** @returns The pixels @p layer plots when its layer stands at @p frame, in
 * order. */
[[nodiscard]] std::vector<rtype::client::ColoredPixel> pixelsPaintedBy(
    const rtype::client::ParticleLayer& layer,
    const rtype::client::LayerFrame& frame);

/** @returns The frames @p layer draws when its layer stands at @p frame, in
 * order. */
[[nodiscard]] std::vector<RecordedFrame> spritesPaintedBy(
    const rtype::client::ParticleLayer& layer,
    const rtype::client::LayerFrame& frame);

/** @returns How many of the particles of @p layer are drawn with the sprite
 * of rank @p sprite. */
[[nodiscard]] int particlesDrawnWith(const rtype::client::ParticleLayer& layer,
                                     std::size_t sprite);
