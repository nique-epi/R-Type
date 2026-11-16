#pragma once

#include "RecordingInterfaceCanvas.hpp"
#include "TabRail.hpp"

namespace rtype::client::testing {

/** @returns The rail of the tests: ONE open, TWO, LOCK locked, at
 * RAIL_TOP_MIDDLE. */
[[nodiscard]] TabRail threeTabRail();

/** @returns What painting @p rail writes and fills. */
[[nodiscard]] InterfaceCanvasRecord railPaintingOf(const TabRail& rail);

}  // namespace rtype::client::testing
