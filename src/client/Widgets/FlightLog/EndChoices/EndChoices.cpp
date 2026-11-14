#include "EndChoices.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <optional>
#include "BarState.hpp"
#include "CutCornerBox.hpp"
#include "EndChoice.hpp"
#include "EndChoiceAreas.hpp"
#include "EntitySprites.hpp"
#include "FlashPreference.hpp"
#include "FlightLogColorConstants.hpp"
#include "FlightLogLayoutConstants.hpp"
#include "FlightLogLook.hpp"
#include "FlightLogSheetConstants.hpp"
#include "FlightLogTextConstants.hpp"
#include "GlowingCall.hpp"
#include "GlowingCallConstants.hpp"
#include "GlowingCallPainting.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

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
  const sf::IntRect quit = endChoiceAreasOf(canvas.textMetrics()).quit;
  CutCornerBox(quit, {.border = QUIT_COLOR, .inside = colors.around})
      .paint(canvas);
  const sf::Vector2i drop(0,
                          state == BarState::Pressed ? PRESSED_TEXT_DROP : 0);
  canvas.drawImage(
      TextureArea{.assetId = FLIGHT_LOG_SHEET_ID,
                  .rectangle = rectangleOf(CROSS_FRAME)},
      ImagePlacement{.topLeft = quit.position + QUIT_CROSS_OFFSET + drop,
                     .tint = colors.ink},
      INTERFACE_AREA);
  canvas.write(QUIT_TEXT, quit.position + QUIT_LABEL_OFFSET + drop,
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
      canvas,
      GlowingCall{
          .text = PROMPT_TEXT,
          .topLeft = endChoiceAreasOf(canvas.textMetrics()).promptTopLeft,
          .sinceTyping = intoPrompt,
          .state = look.replay,
          .steady = look.motion == MotionPreference::Reduced ||
                    look.flashes == FlashPreference::Reduced});
}

EndChoiceAreas endChoiceAreasOf(TextMetrics metrics) {
  if (metrics == TextMetrics{}) {
    return {.quit = QUIT_BUTTON,
            .promptTopLeft = {PROMPT_LEFT, PROMPT_TOP},
            .prompt = PROMPT_AREA};
  }
  const sf::Vector2i quitSize(QUIT_LABEL_OFFSET.x +
                                  regularWidthOf(metrics, QUIT_TEXT) +
                                  QUIT_SPACE_AFTER_LABEL,
                              textBoxHeight(QUIT_SIZE.y, metrics));
  const sf::IntRect quit(
      {QUIT_BUTTON.position.x + QUIT_BUTTON.size.x - quitSize.x,
       QUIT_BUTTON.position.y},
      quitSize);
  const int promptWidth = regularWidthOf(metrics, PROMPT_TEXT);
  const sf::Vector2i prompt(
      (INTERFACE_AREA.size.x - promptWidth) / SIDES_PER_AXIS,
      std::max(PROMPT_TOP,
               quit.position.y + quit.size.y + PROMPT_SPACE_UNDER_QUIT));
  return {.quit = quit,
          .promptTopLeft = prompt,
          .prompt = {
              prompt - CALL_HIT_MARGIN,
              {promptWidth + (SIDES_PER_AXIS * CALL_HIT_MARGIN.x),
               lineHeightOf(metrics) + (SIDES_PER_AXIS * CALL_HIT_MARGIN.y)}}};
}

std::optional<EndChoice> choiceAt(sf::Vector2i pixel, TextMetrics metrics) {
  const EndChoiceAreas areas = endChoiceAreasOf(metrics);
  if (areas.prompt.contains(pixel)) {
    return EndChoice::Replay;
  }
  if (areas.quit.contains(pixel)) {
    return EndChoice::Quit;
  }
  return std::nullopt;
}

}  // namespace rtype::client
