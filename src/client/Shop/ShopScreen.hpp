#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include "AccountLinkConstants.hpp"
#include "AmbienceScope.hpp"
#include "IAudioService.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "IWallet.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "PurchaseTicket.hpp"
#include "ShopScreenParts.hpp"
#include "ShopSelection.hpp"
#include "ShopWares.hpp"
#include "ShopkeeperSpeech.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The shop: the booth of the Bydo shopkeeper, where the player buys
 * looks and hulls with kopecks.
 *
 * The picture of the booth fills the screen. Its locker shows the looks for
 * sale, four rows at a time, and its glass case the hulls, each with its
 * price; the arrows, the wheel and the pointer choose a ware (see
 * ShopSelection). Enter, once per hold, or a click on a ware shows its sales
 * slip (see PurchaseTicket), which takes every event while it shows; its BUY
 * asks the master to buy the ware for the account signed in (see IWallet):
 * the slip ignores every event until the master answers, then is stamped
 * paid, the price gone from the kopecks and the ware in the locker of the
 * hangar, or closes, the status line saying why. A ware the player owns is
 * refused at once, and one the player cannot pay is refused once BUY is
 * pressed, from the balances the master read last. The shop reads the wallet
 * again when it opens. Escape or BACK closes the shop. It hides the screens
 * below.
 *
 * The shopkeeper breathes, blinks and glances around behind his counter,
 * unless the player asked for less motion, and talks in a bubble over his
 * head (see ShopkeeperSpeech): he greets the player, says a line for each
 * purchase and refusal, for a click on his bottles (SHOP_BOTTLES_AREA), which
 * clink, for a click on his abacus or his tea (SHOP_ABACUS_AREA,
 * SHOP_TEA_AREA), and for the radio tuned. The radio shows the frame of a
 * choice under the pointer.
 *
 * The music of the shop plays while it shows, the theme paused. A click on
 * the radio of the counter, or SHOP_RADIO_KEY once per hold, changes it for
 * the next music of the radio: the theme of Papers, Please, then Kavorka,
 * then the music of the shop again. Each loops until the radio is tuned.
 */
class ShopScreen final : public IScreen {
 public:
  ShopScreen(ShopScreenParts parts, std::unique_ptr<IInterfaceCanvas> canvas);

  /** @brief Not copied or moved: its BACK chip captures it. */
  ShopScreen(const ShopScreen&) = delete;
  ShopScreen& operator=(const ShopScreen&) = delete;
  ShopScreen(ShopScreen&&) = delete;
  ShopScreen& operator=(ShopScreen&&) = delete;
  ~ShopScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Moves the choice with an arrow, or buys it once per hold. */
  void keyPressed(const sf::Event::KeyPressed& key);
  /** @brief Chooses the ware under @p position of the window, and buys it
   * when @p clicked. */
  void pointed(sf::Vector2i position, bool clicked);
  /** @brief Shows the sales slip of the chosen ware, or says at once why it
   * cannot be bought when the player owns it or it is not for sale. */
  void offer();
  /** @brief Asks the master to buy the chosen ware, or says on the status
   * line why not.
   * @returns Whether it was asked. */
  bool buy();
  /** @brief Shows the answer of the master to the purchase asked, once it
   * came: the slip stamped paid, or closed with the reason. */
  void settle();
  void repaint();

  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  IAudioService* audio_;
  /** @brief Plays the music of the shop for as long as the shop lives, then
   * gives the screen below its ambience back. */
  AmbienceScope music_;
  IWallet* wallet_;
  MotionPreference motion_;
  /** @brief What the shopkeeper says over his head. */
  ShopkeeperSpeech speech_;
  /** @brief How long the shop was shown: where the shopkeeper is in his
   * breath and in the cycle of his eye. */
  engine::Duration elapsed_{};
  /** @brief Whether the pointer is over the radio of the counter. */
  bool radioPointed_{false};
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  KeyChip backChip_;
  ShopWares wares_;
  ShopSelection selection_;
  /** @brief What the last purchase said, until the choice moves. */
  std::optional<std::string> message_;
  std::optional<sf::Keyboard::Key> heldKey_;
  /** @brief The sales slip of the ware offered, while it shows. */
  PurchaseTicket ticket_;
  /** @brief The purchase the master has not answered yet, NO_ACCOUNT_ASK
   * when none, and the ware it buys. */
  std::uint64_t purchaseAsk_{NO_ACCOUNT_ASK};
  std::size_t purchaseItem_{};
};

}  // namespace rtype::client
