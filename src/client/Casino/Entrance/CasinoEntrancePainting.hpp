#pragma once

#include "EntranceView.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Paints the picture of the front of @p view over the whole
 * interface. */
void paintFront(IInterfaceCanvas& canvas, const EntranceView& view);

/** @brief Paints the butler of @p view in front, his frame cut by the bottom
 * of the picture, then his panel and what he says. */
void paintGreeter(IInterfaceCanvas& canvas, const EntranceView& view);

}  // namespace rtype::client
