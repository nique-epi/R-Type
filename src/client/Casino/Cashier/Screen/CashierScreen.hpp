#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include "AccountLinkConstants.hpp"
#include "AmbienceScope.hpp"
#include "CashierAmount.hpp"
#include "CashierNotice.hpp"
#include "CashierRoom.hpp"
#include "CashierScreenParts.hpp"
#include "CashierWay.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "IWallet.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "SpokenLine.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The cashier of the casino, pushed over the hall, which it hides:
 * the old Bydo teller behind his grille, and the exchange panel, which
 * changes kopecks into chips, or chips back into kopecks, one for one
 * (CashierWay). It opens buying chips; CASHIER_WAY_KEY switches the way, and
 * the rate board on the wall lights the way chosen.
 *
 * The amount stands between two marks: CASHIER_LOWER_KEY and
 * CASHIER_RAISE_KEY, or a click on a mark, move it by the step, one of the
 * chips, which its key 1 to 5, CASHIER_NEXT_STEP_KEY, or a click on it,
 * chooses. Under it, what both balances would hold after the exchange, or
 * why the cashier refuses it (cashierLineOf()); BUY, or CASH OUT, is
 * disabled then.
 *
 * BUY, MENU_CONFIRM_KEY, asks the master for the exchange
 * (IWallet::exchange()); once the master made it, the teller serves it, then
 * counts again, and the cashier stays open for the next one. The teller
 * greets the player, thanks him and apologizes for a refusal in the voice of
 * the Bydo of the shop. BACK,
 * MENU_CANCEL_KEY, closes it, back to the hall. The bars and the switch of
 * the way act once per hold of their key, on its release.
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
  /** @brief Switches the way of the cashier, forgetting what it said. */
  void switchWay();
  /** @brief Asks the master for the exchange; a refused exchange, or one
   * asked while the last is not served yet, does nothing. */
  void confirm();
  /** @brief Has the teller serve the exchange the master made, or says why
   * it did not, once it answered. */
  void settle();
  void repaint();

  IScreenNavigator* navigator_;
  /** @brief Holds the music of the room for as long as it shows. */
  AmbienceScope ambience_;
  ISoundPlayer* sounds_;
  IWallet* wallet_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  MotionPreference motion_;
  CashierRoom room_;
  CashierWay way_{CashierWay::BuyChips};
  CashierAmount amount_;
  /** @brief How long the cashier has been open: where its cage moves. */
  engine::Duration since_{};
  /** @brief How long ago the teller started serving the last exchange made,
   * until he is done. */
  std::optional<engine::Duration> sinceServed_;
  /** @brief What the teller says, in a bubble over his head, in the voice of
   * the Bydo of the shop. */
  SpokenLine tellerLine_;
  KeyChip buyChip_;
  KeyChip cashOutChip_;
  KeyChip backChip_;
  KeyChip wayChip_;
  /** @brief The exchange the master has not answered yet, NO_ACCOUNT_ASK
   * when none. */
  std::uint64_t exchangeAsk_{NO_ACCOUNT_ASK};
  /** @brief What the cashier says in place of the result, if anything. */
  std::optional<CashierNotice> notice_;
};

}  // namespace rtype::client
