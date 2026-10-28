#pragma once

#include "GlowingCall.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @returns What paintGlowingCall() paints for @p call. */
[[nodiscard]] InterfaceCanvasRecord callPaintingOf(
    const rtype::client::GlowingCall& call);
