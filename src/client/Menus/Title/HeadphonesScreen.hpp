#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <optional>
#include <vector>
#include "ColoredPixel.hpp"
#include "HeadphonesScreenConstants.hpp"
#include "HeadphonesScreenParts.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The screen the client opens on: the logo of a pair of headphones and
 * HEADPHONES RECOMMENDED fade in from the ink of the intro in
 * HEADPHONES_FADE_IN, stay HEADPHONES_HOLD, fade back out in
 * HEADPHONES_FADE_OUT, then, after HEADPHONES_INK_BEFORE_NEXT of ink, it asks
 * replaceAll() of the next screen, once.
 *
 * A click, or the release of a key whose press it saw, starts the fade out at
 * once, from the light it had reached. The system repeats the press of a held
 * key, and the intro opens the title on a press during its black second: so
 * this screen acts on the release, never on the press, and while a key whose
 * press it saw is still held it waits in the ink, giving way only once that
 * key is released or the window loses the focus.
 */
class HeadphonesScreen : public IScreen {
 public:
  /** @param canvas Picture the screen is painted on, never null. */
  HeadphonesScreen(HeadphonesScreenParts parts,
                   std::unique_ptr<IInterfaceCanvas> canvas);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  /** @brief Starts the fade out now, from the light the screen has: a leave
   * during the fade in has only that much light left to fade. */
  void leave();

  /** @returns How lit the screen is, from 0, all ink, to 1: the fade in, or
   * the fade out once it started, whichever is darker. */
  [[nodiscard]] float shareOfLight() const;

  /** @brief Paints the screen at the light it has now, and keeps that light
   * in paintedShare_. */
  void repaint();

  std::unique_ptr<IInterfaceCanvas> canvas_;
  IScreenNavigator* navigator_;
  ScreenId next_;
  std::vector<ColoredPixel> logo_;
  engine::Duration since_{};
  engine::Duration leavingFrom_{HEADPHONES_FADE_IN + HEADPHONES_HOLD};
  /** @brief The light the picture was last painted at: it stays exactly 1
   * while the screen stays and exactly 0 in the ink after it, so update()
   * paints only during the fades. */
  float paintedShare_{};
  std::optional<sf::Keyboard::Key> heldKey_;
  bool nextAsked_{};
};

}  // namespace rtype::client
