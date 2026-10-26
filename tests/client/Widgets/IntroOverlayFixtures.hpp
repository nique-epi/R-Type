#pragma once

#include "RecordingInterfaceCanvas.hpp"

/** @returns What paintIntroOverlay() asks a canvas at @p real seconds of the
 * music, with the logo of the title. */
[[nodiscard]] InterfaceCanvasRecord overlayAt(float real);
