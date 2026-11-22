#include "TitleAccountChips.hpp"
#include <SFML/Window/Event.hpp>
#include <string>
#include <utility>
#include "Button.hpp"
#include "IAccountSession.hpp"
#include "IInterfaceCanvas.hpp"
#include "KeyChip.hpp"
#include "KeyChipParts.hpp"
#include "KopecksLabelConstants.hpp"
#include "KopecksPainting.hpp"
#include "Language.hpp"
#include "MenuBuilder.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenConstants.hpp"
#include "TitleAccountChipsParts.hpp"
#include "TitleAccountConstants.hpp"
#include "WalletPurse.hpp"

namespace rtype::client {

namespace {

/** @returns What SIGN IN reads in @p language, with its key. */
Button::Text signInText(Language language) {
  return {.label = std::string(SIGN_IN_LABEL.in(language)),
          .keyName = SIGN_IN_KEY_NAME};
}

/** @returns What SIGN OUT reads in @p language, with its key. */
Button::Text signOutText(Language language) {
  return {.label = std::string(SIGN_OUT_LABEL.in(language)),
          .keyName = SIGN_OUT_KEY_NAME};
}

/** @returns The SIGN IN chip @p parts asks for, top-right. */
KeyChip signInChipOf(const TitleAccountChipsParts& parts) {
  const Button::Text text = signInText(parts.language);
  return KeyChip(KeyChipParts{
      .text = text,
      .area = MenuBuilder::chipArea(text, KOPECKS_BALANCE_TOP_RIGHT.x,
                                    KOPECKS_BALANCE_TOP_RIGHT.y),
      .key = SIGN_IN_KEY,
      .action = parts.signIn,
      .sounds = parts.sounds});
}

/** @returns The SIGN OUT chip @p parts asks for, under the balance. */
KeyChip signOutChipOf(const TitleAccountChipsParts& parts) {
  const Button::Text text = signOutText(parts.language);
  return KeyChip(
      KeyChipParts{.text = text,
                   .area = MenuBuilder::chipArea(
                       text, KOPECKS_BALANCE_TOP_RIGHT.x, TITLE_SIGN_OUT_TOP),
                   .key = SIGN_OUT_KEY,
                   .action = parts.signOut,
                   .sounds = parts.sounds});
}

}  // namespace

TitleAccountChips::TitleAccountChips(TitleAccountChipsParts parts)
    : parts_(std::move(parts)),
      account_(&parts_.account.get()),
      wallet_(&parts_.wallet.get()),
      signIn_(signInChipOf(parts_)),
      signOut_(signOutChipOf(parts_)) {}

void TitleAccountChips::followLanguage(Language language) {
  if (language == parts_.language) {
    return;
  }
  parts_.language = language;
  signIn_ = signInChipOf(parts_);
  signOut_ = signOutChipOf(parts_);
}

bool TitleAccountChips::handleEvent(const sf::Event& event,
                                    const PlayfieldMapping& playfieldPointAt) {
  const bool signedIn = account_->signedIn();
  KeyChip& shown = signedIn ? signOut_ : signIn_;
  KeyChip& hidden = signedIn ? signIn_ : signOut_;
  hidden.handleEvent(sf::Event::FocusLost{}, playfieldPointAt);
  return shown.handleEvent(event, playfieldPointAt);
}

void TitleAccountChips::paint(IInterfaceCanvas& canvas) const {
  if (!account_->signedIn()) {
    signIn_.paint(canvas);
    return;
  }
  paintKopecksBalance(canvas, KOPECKS_BALANCE_TOP_RIGHT,
                      walletKopecks(*wallet_));
  signOut_.paint(canvas);
  canvas.write(account_->nickname(),
               {KOPECKS_BALANCE_TOP_RIGHT.x, TITLE_NICKNAME_TOP},
               TITLE_NICKNAME_STYLE);
}

}  // namespace rtype::client
