#include "EndChoices.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdlib>
#include <optional>
#include <vector>
#include "BarState.hpp"
#include "CutCornerBox.hpp"
#include "EndChoice.hpp"
#include "EntitySprites.hpp"
#include "FlashPreference.hpp"
#include "FlightLogColorConstants.hpp"
#include "FlightLogLayoutConstants.hpp"
#include "FlightLogLook.hpp"
#include "FlightLogSheetConstants.hpp"
#include "FlightLogTextConstants.hpp"
#include "FlightLogTimingConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "Typing.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The colors of the call or of QUIT in one state. */
struct ChoiceColors {
  sf::Color ink;
  sf::Color around;
};

ChoiceColors promptColorsOf(BarState state) {
  switch (state) {
    case BarState::Selected:
      return {.ink = HOVERED_PROMPT_COLOR, .around = HOVERED_PROMPT_GLOW_COLOR};
    case BarState::Pressed:
      return {.ink = PRESSED_PROMPT_COLOR, .around = HOVERED_PROMPT_GLOW_COLOR};
    case BarState::Idle:
    case BarState::Disabled:
      break;
  }
  return {.ink = PROMPT_COLOR, .around = PROMPT_GLOW_COLOR};
}

ChoiceColors quitColorsOf(BarState state) {
  switch (state) {
    case BarState::Selected:
      return {.ink = HOVERED_QUIT_INK, .around = QUIT_COLOR};
    case BarState::Pressed:
      return {.ink = PRESSED_QUIT_INK, .around = PRESSED_QUIT_INSIDE_COLOR};
    case BarState::Idle:
    case BarState::Disabled:
      break;
  }
  return {.ink = QUIT_COLOR, .around = QUIT_INSIDE_COLOR};
}

/** @returns Whether the call is hidden at this step of its blinking: it
 * blinks once typed, while idle, unless motion or flashes are reduced. */
bool isBlinkedOut(const FlightLogLook& look, engine::Duration intoPrompt,
                  std::size_t letters) {
  const bool steady = look.motion == MotionPreference::Reduced ||
                      look.flashes == FlashPreference::Reduced ||
                      look.replay != BarState::Idle ||
                      letters < PROMPT_TEXT.size();
  return !steady && (intoPrompt / BLINK_STEP) % BLINK_STEPS == BLINK_STEPS - 1;
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

/** @brief Writes the letters of the call shown, in the colors of @p state,
 * over a glow of PROMPT_GLOW pixels. */
void paintReplayPrompt(IInterfaceCanvas& canvas, BarState state,
                       std::string_view shown) {
  const ChoiceColors colors = promptColorsOf(state);
  const int drop = state == BarState::Pressed ? PRESSED_TEXT_DROP : 0;
  const sf::Vector2i anchor(PROMPT_LEFT, PROMPT_TOP + drop);
  static const std::vector<sf::Vector2i> glow = glowOffsetsOf(PROMPT_GLOW);
  for (const sf::Vector2i offset : glow) {
    canvas.write(shown, anchor + offset,
                 {.font = TextFont::Regular, .color = colors.around});
  }
  canvas.write(shown, anchor, {.font = TextFont::Regular, .color = colors.ink});
}

void paintQuitButton(IInterfaceCanvas& canvas, BarState state) {
  const ChoiceColors colors = quitColorsOf(state);
  CutCornerBox(QUIT_BUTTON, {.border = QUIT_COLOR, .inside = colors.around})
      .paint(canvas);
  const sf::Vector2i drop(0,
                          state == BarState::Pressed ? PRESSED_TEXT_DROP : 0);
  canvas.drawImage(
      TextureArea{.assetId = FLIGHT_LOG_SHEET_ID,
                  .rectangle = rectangleOf(CROSS_FRAME)},
      ImagePlacement{.topLeft = QUIT_BUTTON.position + QUIT_CROSS_OFFSET + drop,
                     .tint = colors.ink},
      INTERFACE_AREA);
  canvas.write(QUIT_TEXT, QUIT_BUTTON.position + QUIT_LABEL_OFFSET + drop,
               {.font = TextFont::Regular, .color = colors.ink});
}

}  // namespace

void paintEndChoices(IInterfaceCanvas& canvas, const FlightLogLook& look) {
  const engine::Duration intoPrompt =
      look.sinceStart - look.schedule.promptStart;
  if (intoPrompt < engine::Duration::zero()) {
    return;
  }
  paintQuitButton(canvas, look.quit);
  const std::size_t letters =
      lettersTypedAt(intoPrompt, PROMPT_LETTER_TIME, PROMPT_TEXT.size());
  if (!isBlinkedOut(look, intoPrompt, letters)) {
    paintReplayPrompt(canvas, look.replay, typedPrefix(PROMPT_TEXT, letters));
  }
}

std::optional<EndChoice> choiceAt(sf::Vector2i pixel) {
  if (PROMPT_AREA.contains(pixel)) {
    return EndChoice::Replay;
  }
  if (QUIT_BUTTON.contains(pixel)) {
    return EndChoice::Quit;
  }
  return std::nullopt;
}

}  // namespace rtype::client
