#pragma once

#include <optional>
#include "ComponentRegistry.hpp"

namespace rtype::client {

/**
 * @brief Shows on each PaintedBeams the newest action announced at a tick no
 * later than @p renderTick, the tick the client draws, and forgets the older
 * ones; then counts the time of the action shown up to @p renderTick. Nothing
 * changes while the client draws no tick yet.
 */
void showPaintedBeams(engine::ComponentRegistry& components,
                      std::optional<double> renderTick);

}  // namespace rtype::client
