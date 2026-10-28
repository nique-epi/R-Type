#pragma once

#include "NoSignalLook.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @returns What paintNoSignalCard() paints for @p look. */
[[nodiscard]] InterfaceCanvasRecord cardPaintingOf(
    const rtype::client::NoSignalLook& look);
