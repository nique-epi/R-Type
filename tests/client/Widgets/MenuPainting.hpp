#pragma once

#include "Menu.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TextMetrics.hpp"

/** @returns What a menu paints, on a recording canvas. */
[[nodiscard]] InterfaceCanvasRecord paintingOf(const rtype::client::Menu& menu);

/** @returns What @p menu paints on a canvas at the text size @p metrics. */
[[nodiscard]] InterfaceCanvasRecord paintingAt(
    const rtype::client::Menu& menu, rtype::client::TextMetrics metrics);
