#include "CashierScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "AccountLinkConstants.hpp"
#include "CashierAmount.hpp"
#include "CashierConstants.hpp"
#include "CashierExchange.hpp"
#include "CashierPainting.hpp"
#include "CashierRules.hpp"
#include "CashierScreenParts.hpp"
#include "CashierView.hpp"
#include "CashierWay.hpp"
#include "CasinoBarChip.hpp"
#include "ChipRow.hpp"
#include "ChipSelector.hpp"
#include "ExchangeRequest.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfacePixel.hpp"
#include "SoundConstants.hpp"
#include "TimeConstants.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"
#include "WalletRefusal.hpp"

namespace rtype::client {

CashierScreen::CashierScreen(CashierScreenParts parts,
                             std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      wallet_(&parts.wallet.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      way_(parts.way),
      confirmChip_(chipOf(
          parts.way == CashierWay::BuyChips ? BUY_CHIPS_CONFIRM_BAR
                                            : CASH_OUT_CONFIRM_BAR,
          [this] { confirm(); }, parts.sounds)),
      backChip_(chipOf(
          CASHIER_BACK_BAR, [this] { navigator_->pop(); }, parts.sounds)) {
  wallet_->askWallet();
  repaint();
}

void CashierScreen::handleEvent(const sf::Event& event) {
  if (confirmChip_.handleEvent(event, playfieldPointAt_) ||
      backChip_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(*pressed);
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    pointerPressed(interfacePixelAt(playfieldPointAt_(click->position)));
  }
}

void CashierScreen::update([[maybe_unused]] engine::Duration elapsed) {
  settle();
  repaint();
}

void CashierScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool CashierScreen::hidesScreensBelow() const { return false; }

void CashierScreen::keyPressed(const sf::Event::KeyPressed& key) {
  if (key.code == CASHIER_LOWER_KEY) {
    amount_.lower();
  } else if (key.code == CASHIER_RAISE_KEY) {
    amount_.raise();
  } else if (const std::optional<std::size_t> step = chipOfKey(key.code)) {
    amount_.chooseStep(*step);
  } else {
    return;
  }
  sounds_->play(CLICK_SOUND_ID);
}

void CashierScreen::pointerPressed(sf::Vector2i pixel) {
  if (CASHIER_LOWER_MARK_AREA.contains(pixel)) {
    amount_.lower();
  } else if (CASHIER_RAISE_MARK_AREA.contains(pixel)) {
    amount_.raise();
  } else if (const std::optional<std::size_t> chip =
                 chipAt(ChipRow{.firstChip = CASHIER_FIRST_CHIP,
                                .chosen = amount_.step()},
                        pixel)) {
    amount_.chooseStep(*chip);
  } else {
    return;
  }
  sounds_->play(CLICK_SOUND_ID);
}

void CashierScreen::confirm() {
  const std::optional<network::Wallet> balances = wallet_->wallet();
  if (exchangeAsk_ != NO_ACCOUNT_ASK || !balances.has_value() ||
      triedWith(balances).refusal.has_value()) {
    return;
  }
  exchangeAsk_ = wallet_->exchange(exchangeRequestOf(way_, amount_.amount()));
  notice_ = exchangeAsk_ == NO_ACCOUNT_ASK
                ? cashierRefusalLineOf(WalletRefusal::SignedOut)
                : std::string{EXCHANGING_LINE};
}

void CashierScreen::settle() {
  if (exchangeAsk_ == NO_ACCOUNT_ASK) {
    return;
  }
  const std::optional<WalletChangeReading> answer =
      wallet_->exchanged(exchangeAsk_);
  if (!answer.has_value()) {
    return;
  }
  exchangeAsk_ = NO_ACCOUNT_ASK;
  if (answer->refusal.has_value()) {
    notice_ = cashierRefusalLineOf(*answer->refusal);
    return;
  }
  navigator_->pop();
}

CashierExchange CashierScreen::triedWith(
    const std::optional<network::Wallet>& balances) const {
  return exchangeAt(
      balances.value_or(network::Wallet{.kopecks = 0, .chips = 0}), way_,
      amount_.amount());
}

void CashierScreen::repaint() {
  const std::optional<network::Wallet> balances = wallet_->wallet();
  const CashierExchange exchange = triedWith(balances);
  const bool known = balances.has_value();
  confirmChip_.setEnabled(known && exchangeAsk_ == NO_ACCOUNT_ASK &&
                          !exchange.refusal.has_value());
  canvas_->clear();
  paintCashier(
      *canvas_,
      CashierView{
          .way = way_,
          .amount = amount_.amount(),
          .step = amount_.step(),
          .tried = exchange,
          .balances = {.kopecks = balances.has_value()
                                      ? std::optional{balances->kopecks}
                                      : std::nullopt,
                       .chips = balances.has_value()
                                    ? std::optional{balances->chips}
                                    : std::nullopt},
          .notice = known ? notice_
                          : std::optional<std::string>{READING_WALLET_LINE}});
  confirmChip_.paint(*canvas_);
  backChip_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
