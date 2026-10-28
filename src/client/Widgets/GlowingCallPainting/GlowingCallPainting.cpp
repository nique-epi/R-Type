#include "GlowingCallPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdlib>
#include <string_view>
#include <vector>
#include "BarState.hpp"
#include "GlowingCall.hpp"
#include "GlowingCallConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "TextStyle.hpp"
#include "Typing.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The ink of the letters of a call and the glow around them. */
struct CallColors {
  sf::Color ink;
  sf::Color around;
};

/** @returns The colors of a call in a state: gold on dark orange at rest,
 * yellow on gold under the pointer, star white on gold pressed. */
CallColors colorsOf(BarState state) {
  switch (state) {
    case BarState::Selected:
      return {.ink = HOVERED_CALL_COLOR, .around = HOVERED_CALL_GLOW_COLOR};
    case BarState::Pressed:
      return {.ink = PRESSED_CALL_COLOR, .around = HOVERED_CALL_GLOW_COLOR};
    case BarState::Idle:
    case BarState::Disabled:
      break;
  }
  return {.ink = CALL_COLOR, .around = CALL_GLOW_COLOR};
}

/** @returns Whether a call with @p letters typed is hidden now: once typed,
 * at rest and not steady, during the last of every CALL_BLINK_STEPS steps. */
bool isBlinkedOut(const GlowingCall& call, std::size_t letters) {
  const bool steady = call.steady || call.state != BarState::Idle ||
                      letters < letterCountOf(call.text);
  return !steady && (call.sinceTyping / CALL_BLINK_STEP) % CALL_BLINK_STEPS ==
                        CALL_BLINK_STEPS - 1;
}

/** @returns The offsets of a glow of @p radius around a letter: every offset
 * at most @p radius steps away along the grid, the letter itself left out. */
std::vector<sf::Vector2i> glowOffsetsOf(int radius) {
  std::vector<sf::Vector2i> offsets;
  for (int rowOffset = -radius; rowOffset <= radius; ++rowOffset) {
    for (int columnOffset = -radius; columnOffset <= radius; ++columnOffset) {
      const int steps = std::abs(columnOffset) + std::abs(rowOffset);
      if (steps != 0 && steps <= radius) {
        offsets.emplace_back(columnOffset, rowOffset);
      }
    }
  }
  return offsets;
}

}  // namespace

void paintGlowingCall(IInterfaceCanvas& canvas, const GlowingCall& call) {
  const std::size_t letters = lettersShownOf(call);
  if (letters == 0) {
    return;
  }
  const CallColors colors = colorsOf(call.state);
  const int drop = call.state == BarState::Pressed ? PRESSED_TEXT_DROP : 0;
  const sf::Vector2i anchor = call.topLeft + sf::Vector2i(0, drop);
  const std::string_view shown = typedPrefix(call.text, letters);
  static const std::vector<sf::Vector2i> glow = glowOffsetsOf(CALL_GLOW);
  for (const sf::Vector2i offset : glow) {
    canvas.write(shown, anchor + offset,
                 {.font = TextFont::Regular, .color = colors.around});
  }
  canvas.write(shown, anchor, {.font = TextFont::Regular, .color = colors.ink});
}

std::size_t lettersShownOf(const GlowingCall& call) {
  const std::size_t letters = lettersTypedAt(call.sinceTyping, CALL_LETTER_TIME,
                                             letterCountOf(call.text));
  return isBlinkedOut(call, letters) ? 0 : letters;
}

engine::Duration callTypingTimeOf(std::string_view text) {
  return typingTimeOf(CALL_LETTER_TIME, letterCountOf(text));
}

}  // namespace rtype::client
