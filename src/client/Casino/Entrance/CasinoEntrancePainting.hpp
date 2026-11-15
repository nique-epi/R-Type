#pragma once

#include "EntranceView.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Paints @p view, everything the casino shows but its bars: the
 * front, the butler standing on the top step, the panel of the hall with its
 * tag and the balances when the hall shows, then what the butler says. */
void paintEntrance(IInterfaceCanvas& canvas, const EntranceView& view);

}  // namespace rtype::client
