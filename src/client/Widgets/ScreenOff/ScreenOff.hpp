#pragma once

#include "ScreenOffLook.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class IInterfaceCanvas;

/** @returns How far the screen turning off has gone @p sinceGivingUp after the
 * client gave up (see SCREEN_OFF_TIME): the picture squeezed, the line, then
 * nothing. */
[[nodiscard]] ScreenOffLook screenOffAt(engine::Duration sinceGivingUp);

/** @brief Paints the dark of a screen turning off over the whole picture,
 * then the line it became, if any; the squeezed picture is the screen's to
 * draw over this. */
void paintScreenOff(IInterfaceCanvas& canvas, const ScreenOffLook& look);

}  // namespace rtype::client
