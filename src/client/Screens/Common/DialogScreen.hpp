#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "Menu.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief A menu built with MenuBuilder::Frame::Dialog, shown over the dimmed
 * screens below, which stay drawn. What its buttons do is up to the menu it is
 * given. Only the screen on top receives events, so the arrows and Tab stay in
 * the dialog while it is open.
 */
class DialogScreen : public IScreen {
 public:
  /**
   * @param menu The dialog; its actions must not outlive what they capture.
   * @param playfieldPointAt Gives the point of the playfield under a pixel of
   * the window.
   * @param canvas Picture the dialog is painted on, never null.
   */
  DialogScreen(Menu menu, PlayfieldMapping playfieldPointAt,
               std::unique_ptr<IInterfaceCanvas> canvas);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  void repaint();

  std::unique_ptr<IInterfaceCanvas> canvas_;
  PlayfieldMapping playfieldPointAt_;
  Menu menu_;
};

}  // namespace rtype::client
