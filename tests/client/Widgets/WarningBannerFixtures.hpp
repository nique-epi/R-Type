#pragma once

#include <vector>
#include "RecordingInterfaceCanvas.hpp"
#include "WarningBanner.hpp"

/** @returns What paintWarningBanner() paints for @p look. */
[[nodiscard]] InterfaceCanvasRecord warningPaintingOf(
    const rtype::client::WarningBannerLook& look);

/** @returns The lines of @p record written in a band of fire: every writing
 * of the word that carries a clip, in the order written. */
[[nodiscard]] std::vector<RecordedWriting> clippedWritingsOf(
    const InterfaceCanvasRecord& record);
