#include "TitleAccountChips.hpp"
#include <SFML/Window/Event.hpp>
#include <utility>
#include "Button.hpp"
#include "IAccountSession.hpp"
#include "IInterfaceCanvas.hpp"
#include "KeyChip.hpp"
#include "KeyChipParts.hpp"
#include "KopecksLabelConstants.hpp"
#include "KopecksPainting.hpp"
#include "MenuBuilder.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenConstants.hpp"
#include "TitleAccountChipsParts.hpp"
#include "TitleAccountConstants.hpp"
#include "WalletPurse.hpp"

namespace rtype::client {

namespace {

/** @returns What SIGN IN reads, with its key. */
Button::Text signInText() {
  return {.label = SIGN_IN_LABEL, .keyName = SIGN_IN_KEY_NAME};
}

/** @returns What SIGN OUT reads, with its key. */
Button::Text signOutText() {
  return {.label = SIGN_OUT_LABEL, .keyName = SIGN_OUT_KEY_NAME};
}

}  // namespace

TitleAccountChips::TitleAccountChips(TitleAccountChipsParts parts)
    : account_(&parts.account.get()),
      wallet_(&parts.wallet.get()),
      signIn_(KeyChipParts{.text = signInText(),
                           .area = MenuBuilder::chipArea(
                               signInText(), KOPECKS_BALANCE_TOP_RIGHT.x,
                               KOPECKS_BALANCE_TOP_RIGHT.y),
                           .key = SIGN_IN_KEY,
                           .action = std::move(parts.signIn),
                           .sounds = parts.sounds}),
      signOut_(KeyChipParts{
          .text = signOutText(),
          .area = MenuBuilder::chipArea(
              signOutText(), KOPECKS_BALANCE_TOP_RIGHT.x, TITLE_SIGN_OUT_TOP),
          .key = SIGN_OUT_KEY,
          .action = std::move(parts.signOut),
          .sounds = parts.sounds}) {}

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
