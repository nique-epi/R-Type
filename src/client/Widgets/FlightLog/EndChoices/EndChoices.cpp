#include "EndChoices.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
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
#include "GlowingCall.hpp"
#include "GlowingCallPainting.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The colors of QUIT in one state. */
struct ChoiceColors {
  sf::Color ink;
  sf::Color around;
};

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
  paintGlowingCall(
      canvas, GlowingCall{.text = PROMPT_TEXT,
                          .topLeft = {PROMPT_LEFT, PROMPT_TOP},
                          .sinceTyping = intoPrompt,
                          .state = look.replay,
                          .steady = look.motion == MotionPreference::Reduced ||
                                    look.flashes == FlashPreference::Reduced});
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
