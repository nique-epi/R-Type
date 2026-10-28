#include "NoSignalCard.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <span>
#include <string_view>
#include "GlowingCall.hpp"
#include "GlowingCallPainting.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "NoSignalConstants.hpp"
#include "NoSignalLook.hpp"
#include "NoSignalReason.hpp"
#include "Outline.hpp"
#include "TestCardPart.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The rows a strip of the test card covers. */
struct TestCardStrip {
  int top{};
  int height{};
};

/** @returns The column a number of twelfths of a bar reaches from the left
 * edge of the picture, rounded to the nearest pixel. */
int columnOf(int twelfths) {
  return ((twelfths * INTERFACE_AREA.size.x) +
          (TEST_CARD_ROW_TWELFTHS / SIDES_PER_AXIS)) /
         TEST_CARD_ROW_TWELFTHS;
}

/** @brief Paints the parts of a strip side by side from the left edge. */
void paintStrip(IInterfaceCanvas& canvas, std::span<const TestCardPart> parts,
                TestCardStrip strip) {
  int reached = 0;
  for (const TestCardPart& part : parts) {
    const int left = columnOf(reached);
    reached += part.twelfths;
    canvas.fill({{left, strip.top}, {columnOf(reached) - left, strip.height}},
                part.color);
  }
}

/** @returns The line that says why there is no signal. */
std::string_view reasonTextOf(NoSignalReason reason) {
  switch (reason) {
    case NoSignalReason::ClosedServer:
      return CLOSED_SERVER_TEXT;
    case NoSignalReason::SilentServer:
      break;
  }
  return SILENT_SERVER_TEXT;
}

/** @brief Paints the black box over the bars, NO SIGNAL and the reason. */
void paintBox(IInterfaceCanvas& canvas, NoSignalReason reason) {
  canvas.fill(TEST_CARD_BOX, TEST_CARD_BOX_COLOR);
  Outline(TEST_CARD_BOX_OUTLINE, Outline::Line::Solid)
      .paint(canvas, TEST_CARD_OUTLINE_COLOR);
  const int middle = INTERFACE_AREA.size.x / SIDES_PER_AXIS;
  canvas.write(NO_SIGNAL_TEXT,
               {middle, TEST_CARD_BOX.position.y + TEST_CARD_TITLE_DROP},
               {.font = TextFont::Regular,
                .color = NO_SIGNAL_TITLE_COLOR,
                .alignment = TextAlignment::Center});
  canvas.write(reasonTextOf(reason),
               {middle, TEST_CARD_BOX.position.y + TEST_CARD_REASON_DROP},
               {.font = TextFont::Small,
                .color = NO_SIGNAL_REASON_COLOR,
                .alignment = TextAlignment::Center});
}

}  // namespace

void paintNoSignalCard(IInterfaceCanvas& canvas, const NoSignalLook& look) {
  paintStrip(canvas, TEST_CARD_BARS, {.top = 0, .height = TEST_CARD_BAR_ROWS});
  paintStrip(
      canvas, TEST_CARD_CASTELLATIONS,
      {.top = TEST_CARD_BAR_ROWS, .height = TEST_CARD_CASTELLATION_ROWS});
  paintStrip(canvas, TEST_CARD_BOTTOM_ROW,
             {.top = TEST_CARD_BOTTOM_TOP,
              .height = INTERFACE_AREA.size.y - TEST_CARD_BOTTOM_TOP});
  paintBox(canvas, look.reason);
  paintGlowingCall(canvas, reconnectCallOf(look));
}

GlowingCall reconnectCallOf(const NoSignalLook& look) {
  return GlowingCall{.text = RECONNECT_CALL_TEXT,
                     .topLeft = RECONNECT_CALL_TOP_LEFT,
                     .sinceTyping = look.sinceCard - RECONNECT_CALL_DELAY,
                     .state = look.call,
                     .steady = look.steady};
}

}  // namespace rtype::client
