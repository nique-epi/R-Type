#pragma once

#include "IAccountSession.hpp"
#include "IInterfaceCanvas.hpp"
#include "IWallet.hpp"
#include "KeyChip.hpp"
#include "PlayfieldMapping.hpp"
#include "TitleAccountChipsParts.hpp"

namespace sf {
class Event;
}  // namespace sf

namespace rtype::client {

/**
 * @brief The account of the title, in its corners, outside its column of bars:
 * for a guest, SIGN IN in the top-right corner; for a player signed in, the
 * balance there, and SIGN OUT in the bottom-right corner with the nickname
 * over it.
 *
 * Only the chip shown answers an event, so the key of the other one does
 * nothing: SIGN_OUT_KEY means nothing to a guest. The hidden chip is told the
 * focus is lost instead, so it shows no pointer over it, nor a press, once it
 * is shown again.
 */
class TitleAccountChips {
 public:
  explicit TitleAccountChips(TitleAccountChipsParts parts);

  /** @returns Whether the event was the shown chip's, as
   * KeyChip::handleEvent() says. */
  bool handleEvent(const sf::Event& event,
                   const PlayfieldMapping& playfieldPointAt);

  /** @brief Paints the corners: SIGN IN, or the balance, SIGN OUT and the
   * nickname. */
  void paint(IInterfaceCanvas& canvas) const;

  /** @brief Writes the chips again in @p language, when it is not the one
   * they read. */
  void followLanguage(Language language);

 private:
  TitleAccountChipsParts parts_;
  KeyChip signIn_;
  KeyChip signOut_;
};

}  // namespace rtype::client
