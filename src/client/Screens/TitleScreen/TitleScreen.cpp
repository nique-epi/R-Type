#include "TitleScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuFrame.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SquadronConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

TitleScreen::TitleScreen(IScreenNavigator& navigator,
                         std::unique_ptr<IInterfaceCanvas> canvas,
                         PlayfieldMapping playfieldPointAt,
                         MotionPreference motion, std::uint32_t seed)
    : canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(playfieldPointAt)),
      menu_(buildMenu(navigator)),
      squadron_(motion, seed) {
  repaint();
}

void TitleScreen::handleEvent(const sf::Event& event) {
  menu_.handleEvent(event, playfieldPointAt_);
}

void TitleScreen::update(engine::Duration elapsed) {
  menu_.advance(elapsed);
  squadron_.advance(elapsed);
  repaint();
}

void TitleScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

Menu TitleScreen::buildMenu(IScreenNavigator& navigator) {
  return MenuBuilder(MenuFrame::Page)
      .version(GAME_VERSION_TEXT)
      .logo(LOGO_ID, LOGO_SIZE)
      .subtitle(TITLE_SUBTITLE)
      .button({.label = PLAY_LABEL, .description = PLAY_DESCRIPTION},
              [&navigator] { navigator.replaceAll(ScreenId::Game); })
      .button({.label = SETTINGS_LABEL, .description = SETTINGS_DESCRIPTION},
              [&navigator] { navigator.push(ScreenId::Options); })
      .cancelButton(
          {.label = QUIT_LABEL, .description = QUIT_DESCRIPTION},
          [&navigator] { navigator.push(ScreenId::QuitConfirmation); })
      .illustration(SQUADRON_BOX_SIZE)
      .footer(COPYRIGHT_TEXT)
      .build();
}

void TitleScreen::repaint() {
  canvas_->clear();
  menu_.paint(*canvas_);
  const std::optional<sf::IntRect> squadronBox = menu_.illustrationArea();
  if (squadronBox.has_value()) {
    squadron_.paint(*canvas_, squadronBox->position);
  }
  canvas_->display();
}

}  // namespace rtype::client
