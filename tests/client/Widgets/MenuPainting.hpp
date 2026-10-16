#pragma once

#include "Menu.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @returns What a menu paints, on a recording canvas. */
[[nodiscard]] InterfaceCanvasRecord paintingOf(const rtype::client::Menu& menu);
