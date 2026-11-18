#pragma once

#include <vector>
#include "LayerFrame.hpp"
#include "MapLayer.hpp"
#include "RecordedTiles.hpp"

/** @returns A layer of two maps, FIRST_MAP_WIDTH then SECOND_MAP_WIDTH wide,
 * the first with ONE_TILE, cut from WALL_TILESET. */
[[nodiscard]] rtype::client::MapLayer layerOfTwoMaps();

/** @returns A layer whose LDtk layer holds no map. */
[[nodiscard]] rtype::client::MapLayer layerWithoutMaps();

/** @returns The tiles @p layer paints when its layer stands at @p frame,
 * left to right. */
[[nodiscard]] std::vector<RecordedTiles> tilesPaintedBy(
    const rtype::client::MapLayer& layer,
    const rtype::client::LayerFrame& frame);
