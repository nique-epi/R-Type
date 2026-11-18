#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>
#include "ImageLayer.hpp"
#include "LayerFrame.hpp"
#include "RecordedFrame.hpp"

/** @returns The layer of the planet, in a layer that repeats every
 * @p repeat columns. */
[[nodiscard]] rtype::client::ImageLayer planetLayer(float repeat);

/** @returns The frames @p layer paints when its layer stands at @p frame,
 * left to right. */
[[nodiscard]] std::vector<RecordedFrame> framesPaintedBy(
    const rtype::client::ImageLayer& layer,
    const rtype::client::LayerFrame& frame);

/** @returns Where the copies of @p frames that show on the background have
 * their top-left corner: those whose column is from 0 to its width. */
[[nodiscard]] std::vector<sf::Vector2i> cornersOnTheBackground(
    const std::vector<RecordedFrame>& frames);
