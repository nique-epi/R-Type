#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include "AmbienceScope.hpp"
#include "IAudioService.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChip.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingsService.hpp"
#include "ShopScreenParts.hpp"
#include "ShopSelection.hpp"
#include "ShopWares.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The shop: the booth of the Bydo shopkeeper, where the player buys
 * looks and hulls with kopecks.
 *
 * The picture of the booth fills the screen. Its locker shows the looks for
 * sale, four rows at a time, and its glass case the hulls, each with its
 * price; the arrows, the wheel and the pointer choose a ware (see
 * ShopSelection). Enter, once per hold, or a click on a ware buys it: its
 * price leaves the kopecks, the ware joins the locker of the hangar, and the
 * settings are saved; a ware the player owns, or cannot pay, is refused on
 * the status line. Escape or BACK closes the shop. It hides the screens
 * below.
 *
 * The music of the shop plays while it shows, the theme paused. A click on
 * the radio of the counter, or SHOP_RADIO_KEY once per hold, changes it for
 * the music of the radio, and back.
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
  /** @brief Buys the chosen ware, or says on the status line why not. */
  void buy();
  void repaint();
  /** @returns The chosen ware, a position in SHIP_LOOK_ITEM_NAMES. */
  [[nodiscard]] std::size_t chosenItem() const;

  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  IAudioService* audio_;
  /** @brief Plays the music of the shop for as long as the shop lives, then
   * gives the screen below its ambience back. */
  AmbienceScope music_;
  SettingsService* settings_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  KeyChip backChip_;
  ShopWares wares_;
  ShopSelection selection_;
  /** @brief What the last purchase said, until the choice moves. */
  std::optional<std::string> message_;
  std::optional<sf::Keyboard::Key> heldKey_;
};

}  // namespace rtype::client
