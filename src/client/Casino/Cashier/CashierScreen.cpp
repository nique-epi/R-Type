#include "CashierScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "AccountLinkConstants.hpp"
#include "Ambience.hpp"
#include "CashierAmount.hpp"
#include "CashierConstants.hpp"
#include "CashierExchange.hpp"
#include "CashierNotice.hpp"
#include "CashierPainting.hpp"
#include "CashierRoomConstants.hpp"
#include "CashierRoomMoment.hpp"
#include "CashierRoomPainting.hpp"
#include "CashierRules.hpp"
#include "CashierScreenParts.hpp"
#include "CashierView.hpp"
#include "CashierWay.hpp"
#include "CasinoBarChip.hpp"
#include "CasinoSpriteConstants.hpp"
#include "ChipSelector.hpp"
#include "ChipSelectorConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfacePixel.hpp"
#include "SoundConstants.hpp"
#include "SpokenLineParts.hpp"
#include "TimeConstants.hpp"
#include "Wallet.hpp"
#include "WalletChangeReading.hpp"
#include "WalletRefusal.hpp"

namespace rtype::client {

namespace {

/** @returns The place of the chip under @p pixel of the interface; nothing
 * outside the chips. */
std::optional<std::size_t> cashierChipAt(sf::Vector2i pixel) {
  for (std::size_t place = 0; place < CHIP_VALUE_COUNT; ++place) {
    const sf::IntRect chip({CASHIER_FIRST_CHIP.x + (static_cast<int>(place) *
                                                    CHIP_SELECTOR_STRIDE),
                            CASHIER_FIRST_CHIP.y},
                           CHIP_SELECTOR_FRAME_SIZE);
    if (chip.contains(pixel)) {
      return place;
    }
  }
  return std::nullopt;
}

/** @returns The way that is not @p way. */
CashierWay otherWayThan(CashierWay way) {
  return way == CashierWay::BuyChips ? CashierWay::CashOut
                                     : CashierWay::BuyChips;
}

/** @returns The exchange @p amount would make @p way with @p balances, the
 * balances of the wallet; with empty balances while the master has not
 * answered them. */
CashierExchange triedWith(const std::optional<network::Wallet>& balances,
                          CashierWay way, std::uint32_t amount) {
  return exchangeAt(
      balances.value_or(network::Wallet{.kopecks = 0, .chips = 0}), way,
      amount);
}

}  // namespace

CashierScreen::CashierScreen(CashierScreenParts parts,
                             std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      ambience_(parts.audio.get(), Ambience::CasinoTables),
      sounds_(&parts.sounds.get()),
      wallet_(&parts.wallet.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      motion_(parts.motion),
      room_(std::move(parts.room)),
      tellerLine_(SpokenLineParts{.sounds = parts.sounds,
                                  .voice = BYDO_VOICE_SOUND_IDS,
                                  .width = TELLER_BUBBLE_WIDTH,
                                  .tail = TELLER_BUBBLE_TAIL,
                                  .motion = parts.motion,
                                  .stays = false}),
      buyChip_(chipOf(
          BUY_CHIPS_CONFIRM_BAR, [this] { confirm(); }, parts.sounds)),
      cashOutChip_(chipOf(
          CASH_OUT_CONFIRM_BAR, [this] { confirm(); }, parts.sounds)),
      backChip_(chipOf(
          CASHIER_BACK_BAR, [this] { navigator_->pop(); }, parts.sounds)),
      wayChip_(chipOf(CASHIER_WAY_BAR, [this] { switchWay(); }, parts.sounds)) {
  wallet_->askWallet();
  tellerLine_.say(TELLER_GREETING);
  repaint();
}

void CashierScreen::handleEvent(const sf::Event& event) {
  KeyChip& confirmChip = way_ == CashierWay::BuyChips ? buyChip_ : cashOutChip_;
  if (confirmChip.handleEvent(event, playfieldPointAt_) ||
      backChip_.handleEvent(event, playfieldPointAt_) ||
      wayChip_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  if (const auto* pressed = event.getIf<sf::Event::KeyPressed>()) {
    keyPressed(*pressed);
  } else if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
             click != nullptr && click->button == sf::Mouse::Button::Left) {
    pointerPressed(interfacePixelAt(playfieldPointAt_(click->position)));
  }
}

void CashierScreen::update(engine::Duration elapsed) {
  since_ += elapsed;
  tellerLine_.advance(elapsed);
  if (sinceServed_.has_value()) {
    *sinceServed_ += elapsed;
    if (!tellerServesAt(CashierRoomMoment{.since = since_,
                                          .sinceServed = sinceServed_,
                                          .way = way_,
                                          .motion = motion_})) {
      sinceServed_.reset();
      notice_.reset();
    }
  }
  settle();
  repaint();
}

void CashierScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool CashierScreen::hidesScreensBelow() const { return true; }

void CashierScreen::keyPressed(const sf::Event::KeyPressed& key) {
  if (key.code == CASHIER_LOWER_KEY) {
    amount_.lower();
  } else if (key.code == CASHIER_RAISE_KEY) {
    amount_.raise();
  } else if (key.code == CASHIER_NEXT_STEP_KEY) {
    amount_.chooseStep((amount_.step() + 1) % CHIP_VALUE_COUNT);
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
  } else if (const std::optional<std::size_t> chip = cashierChipAt(pixel)) {
    amount_.chooseStep(*chip);
  } else {
    return;
  }
  sounds_->play(CLICK_SOUND_ID);
}

void CashierScreen::switchWay() {
  way_ = otherWayThan(way_);
  if (!sinceServed_.has_value() && exchangeAsk_ == NO_ACCOUNT_ASK) {
    notice_.reset();
  }
}

void CashierScreen::confirm() {
  const std::optional<network::Wallet> balances = wallet_->wallet();
  if (exchangeAsk_ != NO_ACCOUNT_ASK || sinceServed_.has_value() ||
      !balances.has_value() ||
      triedWith(balances, way_, amount_.amount()).refusal.has_value()) {
    return;
  }
  exchangeAsk_ = wallet_->exchange(exchangeRequestOf(way_, amount_.amount()));
  notice_ = exchangeAsk_ == NO_ACCOUNT_ASK
                ? CashierNotice{.text = cashierRefusalLineOf(
                                    WalletRefusal::SignedOut),
                                .refusal = true}
                : CashierNotice{.text = EXCHANGING_LINE, .refusal = false};
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
    notice_ = CashierNotice{.text = cashierRefusalLineOf(*answer->refusal),
                            .refusal = true};
    tellerLine_.say(TELLER_REFUSED_LINE);
    return;
  }
  sinceServed_ = engine::Duration::zero();
  tellerLine_.say(TELLER_SERVED_LINE);
}

void CashierScreen::repaint() {
  const std::optional<network::Wallet> balances = wallet_->wallet();
  const CashierExchange exchange = triedWith(balances, way_, amount_.amount());
  KeyChip& confirmChip = way_ == CashierWay::BuyChips ? buyChip_ : cashOutChip_;
  confirmChip.setEnabled(
      balances.has_value() && exchangeAsk_ == NO_ACCOUNT_ASK &&
      !sinceServed_.has_value() && !exchange.refusal.has_value());
  canvas_->clear();
  paintCashierRoom(*canvas_, room_,
                   CashierRoomMoment{.since = since_,
                                     .sinceServed = sinceServed_,
                                     .way = way_,
                                     .motion = motion_});
  tellerLine_.paint(*canvas_, TELLER_BUBBLE_BOTTOM_LEFT);
  paintCashierPanel(*canvas_,
                    CashierView{.way = way_,
                                .amount = amount_.amount(),
                                .step = amount_.step(),
                                .tried = exchange,
                                .notice = balances.has_value()
                                              ? notice_
                                              : std::optional{CashierNotice{
                                                    .text = READING_WALLET_LINE,
                                                    .refusal = false}}});
  confirmChip.paint(*canvas_);
  backChip_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
