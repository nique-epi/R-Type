#pragma once

#include "Menu.hpp"
#include "RecordingRowControl.hpp"

/** @returns A tabbed panel of one recording row described "> SIZES", then
 * BACK, its cancel button, then QUIT, asked for after it. */
[[nodiscard]] rtype::client::Menu panelWithQuitAfterBack(
    RowControlRecord& record);
