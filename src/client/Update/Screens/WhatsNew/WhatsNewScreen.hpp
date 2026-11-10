#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <functional>
#include <memory>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "IScreenNavigator.hpp"
#include "ReleaseNotes.hpp"
#include "TimeConstants.hpp"
#include "WhatsNewScreenParts.hpp"

namespace rtype::client {

/**
 * @brief The notes of the release this client was just updated to, in a
 * panel over the dimmed title, once: MENU_CONFIRM_KEY, MENU_CANCEL_KEY or a
 * click closes it and marks them read. A key held down closes it once, as on
 * the end screen.
 */
class WhatsNewScreen : public IScreen {
 public:
  /** @param canvas Picture the panel is painted on, never null. */
  WhatsNewScreen(WhatsNewScreenParts parts,
                 std::unique_ptr<IInterfaceCanvas> canvas);

  void handleEvent(const sf::Event& event) override;
  void update(engine::Duration elapsed) override;
  void draw(sf::RenderTarget& target) const override;
  [[nodiscard]] bool hidesScreensBelow() const override;

 private:
  void close();
  void paint();

  std::unique_ptr<IInterfaceCanvas> canvas_;
  IScreenNavigator* navigator_;
  ReleaseNotes notes_;
  std::function<void()> onClose_;
  std::optional<sf::Keyboard::Key> heldKey_;
  bool closed_{};
};

}  // namespace rtype::client
