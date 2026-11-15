#include "CashierPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include "BackgroundColorConstants.hpp"
#include "CashierConstants.hpp"
#include "CashierRules.hpp"
#include "CashierView.hpp"
#include "CashierWay.hpp"
#include "CasinoBalances.hpp"
#include "CasinoPainting.hpp"
#include "CasinoPaintingConstants.hpp"
#include "ChipRow.hpp"
#include "ChipSelector.hpp"
#include "ChipSelectorConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "Kopecks.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

TextStyle smallStyle(sf::Color color, TextAlignment alignment) {
  return {.font = TextFont::Small, .color = color, .alignment = alignment};
}

TextStyle regularStyle(sf::Color color, TextAlignment alignment) {
  return {.font = TextFont::Regular, .color = color, .alignment = alignment};
}

/** @returns The name of the balance @p way takes from, then of the one it
 * gives to, as @p source says. */
const char* balanceWordOf(CashierWay way, bool source) {
  return (way == CashierWay::BuyChips) == source ? KOPECKS_WORD : CHIPS_WORD;
}

/** @brief Writes the balance @p view takes from, on the left, when
 * @p source, or the one it gives to, on the right: its name in small print
 * over its amount, gold for the kopecks, white for the chips. */
void paintBalance(IInterfaceCanvas& canvas, const CashierView& view,
                  bool source) {
  const char* word = balanceWordOf(view.way, source);
  const std::uint32_t amount = source ? sourceOf(view.purse, view.way)
                                      : destinationOf(view.purse, view.way);
  const TextAlignment alignment =
      source ? TextAlignment::Left : TextAlignment::Right;
  const int column = source ? CASHIER_LEFT : CASHIER_RIGHT;
  const sf::Color color =
      std::string(word) == KOPECKS_WORD ? STAR_GOLD : STAR_WHITE;
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
               regularStyle(HUD_CYAN, TextAlignment::Center));
  paintBalance(canvas, view, false);
  canvas.fill(CASHIER_UPPER_RULE, CASINO_DARK_GOLD);
}

/** @brief Writes a mark that moves the amount, centered across @p area. */
void paintMark(IInterfaceCanvas& canvas, const char* mark, sf::IntRect area) {
  canvas.write(mark,
               {area.position.x + (area.size.x / SIDES_PER_AXIS),
                CASHIER_AMOUNT_TEXT_TOP},
               regularStyle(HUD_CYAN, TextAlignment::Center));
}

/** @brief Paints the amount in its box between its marks, its name on the
 * left and the step on the right. */
void paintAmount(IInterfaceCanvas& canvas, const CashierView& view) {
  canvas.write(CASHIER_AMOUNT_CAPTION, {CASHIER_LEFT, CASHIER_AMOUNT_TOP},
               smallStyle(PALE_BLUE, TextAlignment::Left));
  const sf::Vector2i border(CASHIER_AMOUNT_BORDER, CASHIER_AMOUNT_BORDER);
  canvas.fill(CASHIER_AMOUNT_BOX, CASINO_GOLD);
  canvas.fill({CASHIER_AMOUNT_BOX.position + border,
               CASHIER_AMOUNT_BOX.size - (border * SIDES_PER_AXIS)},
              INK);
  canvas.write(std::to_string(view.amount),
               {CASHIER_MIDDLE, CASHIER_AMOUNT_TEXT_TOP},
               regularStyle(STAR_GOLD, TextAlignment::Center));
  paintMark(canvas, CASHIER_LOWER_MARK, CASHIER_LOWER_MARK_AREA);
  paintMark(canvas, CASHIER_RAISE_MARK, CASHIER_RAISE_MARK_AREA);
  canvas.write(CASHIER_STEP_CAPTION + std::to_string(CHIP_VALUES.at(view.step)),
               {CASHIER_RIGHT, CASHIER_AMOUNT_TOP},
               smallStyle(PALE_BLUE, TextAlignment::Right));
}

/** @brief Paints what follows the exchange under the lower rule: the rate,
 * then both balances after it, or why it is refused. */
void paintAfter(IInterfaceCanvas& canvas, const CashierView& view) {
  canvas.fill(CASHIER_LOWER_RULE, CASINO_DARK_GOLD);
  canvas.write(CASHIER_AFTER_CAPTION, {CASHIER_LEFT, CASHIER_AFTER_TOP},
               smallStyle(PALE_BLUE, TextAlignment::Left));
  canvas.write(
      view.way == CashierWay::BuyChips ? BUY_CHIPS_RATE : CASH_OUT_RATE,
      {CASHIER_MIDDLE, CASHIER_AFTER_TOP},
      smallStyle(PALE_BLUE, TextAlignment::Center));
  if (view.tried.refusal.has_value()) {
    canvas.write(cashierLineOf(view.tried, view.way, view.amount),
                 {CASHIER_MIDDLE, CASHIER_RESULT_TOP},
                 smallStyle(HUD_CYAN, TextAlignment::Center));
    return;
  }
  for (const bool source : {true, false}) {
    const std::uint32_t after = source
                                    ? sourceOf(view.tried.purse, view.way)
                                    : destinationOf(view.tried.purse, view.way);
    canvas.write(kopecksText(after) + CASHIER_AMOUNT_WORD_GAP +
                     balanceWordOf(view.way, source),
                 {source ? CASHIER_LEFT : CASHIER_RIGHT, CASHIER_RESULT_TOP},
                 smallStyle(STAR_WHITE, source ? TextAlignment::Left
                                               : TextAlignment::Right));
  }
}

}  // namespace

void paintCashier(IInterfaceCanvas& canvas, const CashierView& view) {
  canvas.fill(INTERFACE_AREA, DIM_LAYER_COLOR);
  paintCasinoBalances(canvas, CasinoBalances{.kopecks = view.purse.kopecks,
                                             .chips = view.purse.chips});
  paintGoldPanel(canvas, CASHIER_PANEL_AREA);
  paintNameTag(
      canvas, CASHIER_TAG_TOP_LEFT,
      view.way == CashierWay::BuyChips ? BUY_CHIPS_TITLE : CASH_OUT_TITLE);
  paintExchangedBalances(canvas, view);
  paintAmount(canvas, view);
  paintChipRow(canvas,
               ChipRow{.firstChip = CASHIER_FIRST_CHIP, .chosen = view.step});
  paintAfter(canvas, view);
}

}  // namespace rtype::client
