#include "CashierPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include "BackgroundColorConstants.hpp"
#include "CashierAttempt.hpp"
#include "CashierConstants.hpp"
#include "CashierNotice.hpp"
#include "CashierRules.hpp"
#include "CashierView.hpp"
#include "CashierWay.hpp"
#include "CasinoPainting.hpp"
#include "CasinoPaintingConstants.hpp"
#include "CasinoSpriteConstants.hpp"
#include "ChipSelectorConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "Kopecks.hpp"
#include "TextMarks.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "Translation.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

TextStyle smallStyle(sf::Color color, TextAlignment alignment) {
  return {.font = TextFont::Small, .color = color, .alignment = alignment};
}

TextStyle regularStyle(sf::Color color, TextAlignment alignment) {
  return {.font = TextFont::Regular, .color = color, .alignment = alignment};
}

/** @returns Whether the balance @p way takes from, or the one it gives to,
 * as @p source says, is the kopecks. */
bool isKopecks(CashierWay way, bool source) {
  return (way == CashierWay::BuyChips) == source;
}

/** @returns The name of the balance @p way takes from, then of the one it
 * gives to, as @p source says. */
Translation balanceWordOf(CashierWay way, bool source) {
  return isKopecks(way, source) ? KOPECKS_WORD : CHIPS_WORD;
}

/** @brief Writes the balance @p view takes from, on the left, when
 * @p source, or the one it gives to, on the right: its name in small print
 * over its amount, gold for the kopecks, white for the chips. */
void paintBalance(IInterfaceCanvas& canvas, const CashierView& view,
                  bool source) {
  const std::string_view word =
      balanceWordOf(view.way, source).in(canvas.language());
  const std::uint32_t amount = source
                                   ? sourceOf(view.tried.wallet, view.way)
                                   : destinationOf(view.tried.wallet, view.way);
  const TextAlignment alignment =
      source ? TextAlignment::Left : TextAlignment::Right;
  const int column = source ? CASHIER_LEFT : CASHIER_RIGHT;
  const sf::Color color = isKopecks(view.way, source) ? STAR_GOLD : STAR_WHITE;
  canvas.write(word, {column, CASHIER_CAPTION_TOP},
               smallStyle(PALE_BLUE, alignment));
  canvas.write(kopecksText(amount), {column, CASHIER_BALANCE_TOP},
               regularStyle(color, alignment));
}

/** @brief Paints the two balances the cashier changes and the marks between
 * them, then the rule under them. */
void paintExchangedBalances(IInterfaceCanvas& canvas, const CashierView& view) {
  paintBalance(canvas, view, true);
  canvas.write(CASHIER_TOWARDS_MARK, {CASHIER_MIDDLE, CASHIER_TOWARDS_MARK_TOP},
               regularStyle(CASINO_LIGHT_GOLD, TextAlignment::Center));
  paintBalance(canvas, view, false);
  canvas.fill(CASHIER_UPPER_RULE, CASINO_DARK_GOLD);
}

/** @brief Paints the amount in its box between its marks, its name on the
 * left and the step on the right. */
void paintAmount(IInterfaceCanvas& canvas, const CashierView& view) {
  canvas.write(CASHIER_AMOUNT_CAPTION.in(canvas.language()),
               {CASHIER_LEFT, CASHIER_AMOUNT_TOP},
               smallStyle(PALE_BLUE, TextAlignment::Left));
  const sf::Vector2i border(CASHIER_AMOUNT_BORDER, CASHIER_AMOUNT_BORDER);
  canvas.fill(CASHIER_AMOUNT_BOX, CASINO_GOLD);
  canvas.fill({CASHIER_AMOUNT_BOX.position + border,
               CASHIER_AMOUNT_BOX.size - (border * SIDES_PER_AXIS)},
              CASINO_PANEL_INSIDE_COLOR);
  canvas.write(std::to_string(view.amount),
               {CASHIER_MIDDLE, CASHIER_AMOUNT_TEXT_TOP},
               regularStyle(STAR_WHITE, TextAlignment::Center));
  canvas.write(CASHIER_LOWER_MARK,
               {CASHIER_LOWER_MARK_RIGHT, CASHIER_AMOUNT_TEXT_TOP},
               regularStyle(CASINO_LIGHT_GOLD, TextAlignment::Right));
  canvas.write(CASHIER_RAISE_MARK,
               {CASHIER_RAISE_MARK_LEFT, CASHIER_AMOUNT_TEXT_TOP},
               regularStyle(CASINO_LIGHT_GOLD, TextAlignment::Left));
  canvas.write(withMark(std::string(CASHIER_STEP_LINE.in(canvas.language())),
                        {.mark = CASHIER_STEP_MARK,
                         .value = std::to_string(CHIP_VALUES.at(view.step))}),
               {CASHIER_RIGHT, CASHIER_AMOUNT_TOP},
               smallStyle(PALE_BLUE, TextAlignment::Right));
}

/** @brief Paints the five chips side by side, the chosen one framed, never
 * raised. */
void paintChips(IInterfaceCanvas& canvas, std::size_t chosen) {
  for (std::size_t place = 0; place < CHIP_VALUE_COUNT; ++place) {
    const sf::Vector2i topLeft(
        CASHIER_FIRST_CHIP.x + (static_cast<int>(place) * CHIP_SELECTOR_STRIDE),
        CASHIER_FIRST_CHIP.y);
    canvas.drawImage(
        TextureArea{
            .assetId = CHIP_SELECTOR_SHEET_ID,
            .rectangle = {{static_cast<int>(place) * CHIP_SELECTOR_STRIDE, 0},
                          CHIP_SELECTOR_FRAME_SIZE}},
        ImagePlacement{.topLeft = topLeft}, INTERFACE_AREA);
    if (place == chosen) {
      const sf::Vector2i margin(CASHIER_CHOSEN_CHIP_MARGIN,
                                CASHIER_CHOSEN_CHIP_MARGIN);
      paintEdges(canvas,
                 {topLeft - margin,
                  CHIP_SELECTOR_FRAME_SIZE + (margin * SIDES_PER_AXIS)},
                 CASINO_LIGHT_GOLD);
    }
  }
}

/** @brief Writes what both balances hold after the exchange of @p view. */
void paintBalancesAfter(IInterfaceCanvas& canvas, const CashierView& view) {
  for (const bool source : {true, false}) {
    const std::uint32_t after =
        source ? sourceOf(view.tried.wallet, view.way)
               : destinationOf(view.tried.wallet, view.way);
    canvas.write(
        kopecksText(after) + CASHIER_AMOUNT_WORD_GAP +
            std::string(balanceWordOf(view.way, source).in(canvas.language())),
        {source ? CASHIER_LEFT : CASHIER_RIGHT, CASHIER_RESULT_TOP},
        regularStyle(STAR_WHITE,
                     source ? TextAlignment::Left : TextAlignment::Right));
  }
}

/** @brief Paints what follows the exchange under the lower rule: AFTER and
 * the rate, then both balances after it, or what the cashier says in their
 * place. */
void paintAfter(IInterfaceCanvas& canvas, const CashierView& view) {
  canvas.fill(CASHIER_LOWER_RULE, CASINO_DARK_GOLD);
  canvas.write(CASHIER_AFTER_CAPTION.in(canvas.language()),
               {CASHIER_LEFT, CASHIER_AFTER_TOP},
               smallStyle(PALE_BLUE, TextAlignment::Left));
  canvas.write(
      (view.way == CashierWay::BuyChips ? BUY_CHIPS_RATE : CASH_OUT_RATE)
          .in(canvas.language()),
      {CASHIER_RIGHT, CASHIER_AFTER_TOP},
      smallStyle(PALE_BLUE, TextAlignment::Right));
  if (!view.notice.has_value() && !view.tried.refusal.has_value()) {
    paintBalancesAfter(canvas, view);
    return;
  }
  const CashierNotice notice = view.notice.value_or(CashierNotice{
      .text = cashierLineOf(
          CashierAttempt{
              .tried = view.tried, .way = view.way, .amount = view.amount},
          canvas.language()),
      .refusal = true});
  canvas.write(notice.text, {CASHIER_MIDDLE, CASHIER_NOTICE_TOP},
               smallStyle(notice.refusal ? CASINO_NEON_RED : STAR_GOLD,
                          TextAlignment::Center));
}

}  // namespace

void paintCashierPanel(IInterfaceCanvas& canvas, const CashierView& view) {
  paintGoldPanel(canvas, CASHIER_PANEL_AREA);
  paintNameTag(
      canvas, CASHIER_TAG_TOP_LEFT,
      (view.way == CashierWay::BuyChips ? BUY_CHIPS_TITLE : CASH_OUT_TITLE)
          .in(canvas.language()));
  paintExchangedBalances(canvas, view);
  paintAmount(canvas, view);
  paintChips(canvas, view.step);
  paintAfter(canvas, view);
}

}  // namespace rtype::client
