#pragma once

#include <string_view>
#include "GlowingCall.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class IInterfaceCanvas;

/**
 * @brief Writes @p call as the end screen writes PRESS ENTER TO REPLAY: the
 * letters typed by now, one more every CALL_LETTER_TIME from its first, in
 * the regular font over a glow of CALL_GLOW pixels, in the colors of its
 * state (GlowingCallConstants.hpp), PRESSED_TEXT_DROP lower while pressed.
 * Once every letter is typed, a call at rest that is not steady is hidden
 * the last step of every CALL_BLINK_STEPS steps of CALL_BLINK_STEP. Nothing
 * is written before its first letter.
 */
void paintGlowingCall(IInterfaceCanvas& canvas, const GlowingCall& call);

/** @returns How long a call of @p text takes to type, from its first letter
 * to its last, CALL_LETTER_TIME a letter. */
[[nodiscard]] engine::Duration callTypingTimeOf(std::string_view text);

}  // namespace rtype::client
