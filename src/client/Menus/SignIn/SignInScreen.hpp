#pragma once

#include <cstdint>
#include <memory>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "Menu.hpp"
#include "SignInLines.hpp"
#include "SignInScreenParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The dialog that signs the game in through the web site, the OAuth
 * 2.0 Device Authorization Grant of RFC 8628: it asks for a link when built,
 * opens its page in the browser once the code came, and shows the code, the
 * page and the minutes it stays valid, so a player may also validate it from
 * another device. Once a player approved it, it signs the account session in
 * with the token, and once the account is read, closes itself and opens
 * SignInReturn::screen again in place of the dialog that asked.
 *
 * BACK, which MENU_CANCEL_KEY answers too, forgets the link and closes the
 * dialog; NEW CODE, shown once a link ended without a session, asks for
 * another. Its menu is built again only when what it shows changes, so the
 * selection of the player stays in between.
 */
class SignInScreen final : public IScreen {
 public:
  SignInScreen(SignInScreenParts parts,
               std::unique_ptr<IInterfaceCanvas> canvas);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Opens the page of a code once, and signs in once the link gave
   * a token. */
  void followLink();

  /** @brief Closes the dialog and opens the screen that asked, once. */
  void leaveSignedIn();

  /** @brief Builds the menu again when @p lines differ from those shown. */
  void show(const SignInLines& lines);

  void askNewCode();
  void back() const;
  void repaint();

  SignInScreenParts parts_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  Menu menu_;
  SignInLines shown_;
  std::uint64_t ask_{};
  bool pageOpened_{false};
  bool signInAsked_{false};
  bool left_{false};
};

}  // namespace rtype::client
