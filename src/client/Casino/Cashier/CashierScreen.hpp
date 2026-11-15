#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include "CashierAmount.hpp"
#include "CashierScreenParts.hpp"
#include "CashierWay.hpp"
#include "Exchange.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChip.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingsService.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The cashier of the casino, pushed over the hall, which shows
 * dimmed under it: it changes kopecks into chips, or chips back into
 * kopecks, one for one (CashierWay).
 *
 * The amount stands between two marks: CASHIER_LOWER_KEY and
 * CASHIER_RAISE_KEY, or a click on a mark, move it by the step, one of the
 * chips of the selector, which its key 1 to 5, or a click on it, chooses.
 * Under it, what both balances would hold after the exchange, or why the
 * cashier refuses it (cashierLineOf()); BUY, or CASH OUT, is disabled then.
 *
 * BUY, MENU_CONFIRM_KEY, makes the exchange whole (buyChips() or
 * cashOutChips()), saves the settings, and closes the cashier, back to the
 * hall; BACK, MENU_CANCEL_KEY, closes it without exchanging. Both act once
 * per hold of their key, on its release.
 */
class CashierScreen final : public IScreen {
 public:
  /** @param canvas Picture the cashier is painted on, never null. */
  CashierScreen(CashierScreenParts parts,
                std::unique_ptr<IInterfaceCanvas> canvas);

  /** @brief The actions of the bars point at the screen, so it never
   * moves. */
  CashierScreen(const CashierScreen&) = delete;
  CashierScreen& operator=(const CashierScreen&) = delete;
  CashierScreen(CashierScreen&&) = delete;
  CashierScreen& operator=(CashierScreen&&) = delete;
  ~CashierScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Moves the amount, or chooses the step, as @p key says. */
  void keyPressed(const sf::Event::KeyPressed& key);
  /** @brief Moves the amount, or chooses the step, as the mark or the chip
   * under @p pixel of the interface says. */
  void pointerPressed(sf::Vector2i pixel);
  /** @brief Makes the exchange, saves the settings, then closes the cashier;
   * a refused exchange does nothing. */
  void confirm();
  /** @returns The exchange the amount would make with the purse of the
   * settings. */
  [[nodiscard]] Exchange tried() const;
  void repaint();

  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  SettingsService* settings_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  CashierWay way_;
  CashierAmount amount_;
  KeyChip confirmChip_;
  KeyChip backChip_;
};

}  // namespace rtype::client
