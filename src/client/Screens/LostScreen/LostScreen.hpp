#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "LostScreenLook.hpp"
#include "LostScreenParts.hpp"
#include "NoSignalReason.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The screen the frame loop puts in place of every other once the
 * session with the server ended: the last picture they showed turns off like
 * an old screen (see screenOffAt()), then the NO SIGNAL card shows the SMPTE
 * color bars, why the signal is gone and PRESS ENTER TO RECONNECT (see
 * paintNoSignalCard()). It hides the screens below and paints its picture
 * only when what it shows changes (see LostScreenLook).
 *
 * MENU_CONFIRM_KEY, or a left click over RECONNECT_CALL_AREA, presses the
 * call: a click sounds, the call shows pressed for PRESS_DURATION, then the
 * title replaces every screen and the connection screen opens over it, its
 * fields those of the lost game. MENU_CONFIRM_KEY or a left click anywhere
 * while the screen turns off shows the card at once instead. MENU_CANCEL_KEY
 * leaves for the title. A key held down acts once. Once it asked to leave, it
 * asks nothing more. With reduced motion, it opens on the card with the whole
 * call, which never blinks; with reduced flashes, the call never blinks
 * either.
 */
class LostScreen : public IScreen {
 public:
  /** @param canvas Picture the card is painted on, never null. */
  LostScreen(LostScreenParts parts, std::unique_ptr<IInterfaceCanvas> canvas);

  LostScreen(const LostScreen&) = delete;
  LostScreen& operator=(const LostScreen&) = delete;
  LostScreen(LostScreen&&) = delete;
  LostScreen& operator=(LostScreen&&) = delete;
  ~LostScreen() override = default;

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Lights the call under the pointer, or presses it on a left
   * click over it; while the screen turns off, any left click shows the
   * card. */
  void handlePointer(const sf::Event& event);

  /** @brief Shows the card at once while the screen turns off; on the card,
   * presses the call, once, with the click of the menus. */
  void reconnect();

  /** @brief Asks for the title in place of every screen, once. */
  void quit();

  /** @brief Once the call showed pressed for PRESS_DURATION, asks for the
   * title in place of every screen, then the connection screen over it. */
  void advanceExit(engine::Duration elapsed);

  /** @brief Paints the screen turning off, or the card, for the time since
   * it was built, unless the picture already shows that look. */
  void repaint();

  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  std::unique_ptr<IInterfaceCanvas> canvas_;
  sf::Texture picture_;
  NoSignalReason reason_;
  PlayfieldMapping playfieldPointAt_;
  /** @brief Whether the call never blinks: motion or flashes reduced. */
  bool steady_ = false;
  engine::Duration sinceStart_{};
  std::optional<LostScreenLook> painted_;
  std::optional<engine::Duration> pressedFor_;
  std::optional<sf::Keyboard::Key> heldKey_;
  bool hovered_ = false;
  bool leaving_ = false;
};

}  // namespace rtype::client
