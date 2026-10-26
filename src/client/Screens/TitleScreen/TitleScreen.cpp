#include "TitleScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <optional>
#include <utility>
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SquadronConstants.hpp"
#include "TimeConstants.hpp"
#include "TitleScreenParts.hpp"

namespace rtype::client {

TitleScreen::TitleScreen(TitleScreenParts parts,
                         std::unique_ptr<IInterfaceCanvas> canvas)
    : canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      menu_(buildMenu(parts.navigator.get(), parts.sounds.get())),
      squadron_(parts.motion, parts.seed, parts.squadronStart) {
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

bool TitleScreen::hidesScreensBelow() const { return true; }

Menu TitleScreen::buildMenu(IScreenNavigator& navigator, ISoundPlayer& sounds) {
  return MenuBuilder(MenuBuilder::Frame::Page, &sounds)
      .version(GAME_VERSION_TEXT)
      .logo(LOGO_ID, LOGO_SIZE)
      .subtitle(TITLE_SUBTITLE)
      .button({.label = PLAY_LABEL, .description = PLAY_DESCRIPTION},
              [&navigator] { navigator.push(ScreenId::Connection); })
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
