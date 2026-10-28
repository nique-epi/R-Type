#pragma once

#include "GlowingCall.hpp"
#include "NoSignalLook.hpp"

namespace rtype::client {

class IInterfaceCanvas;

/**
 * @brief Paints the NO SIGNAL card over the whole picture: the SMPTE color
 * bars of a television with no signal (NoSignalConstants.hpp), then a black
 * box in the middle of the bars that says NO SIGNAL and why, and PRESS ENTER
 * TO RECONNECT, written as every call (see paintGlowingCall())
 * RECONNECT_CALL_DELAY after the card showed.
 */
void paintNoSignalCard(IInterfaceCanvas& canvas, const NoSignalLook& look);

/** @returns The call to reconnect the card shows in @p look, as
 * paintNoSignalCard() writes it: typed from RECONNECT_CALL_DELAY after the
 * card showed. */
[[nodiscard]] GlowingCall reconnectCallOf(const NoSignalLook& look);

}  // namespace rtype::client
