#include "TitleScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <optional>
#include <utility>
#include "ClientVersionConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "KopecksLabelConstants.hpp"
#include "KopecksPainting.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlayStart.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SquadronConstants.hpp"
#include "TextMeasures.hpp"
#include "TimeConstants.hpp"
#include "TitleScreenParts.hpp"
#include "UpdateScreenIdConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The screen PLAY opens: the connection to the server given at
 * launch, or the server map. */
ScreenId playScreenOf(PlayStart playStart) {
  return playStart == PlayStart::GivenServer
             ? CONNECTION_TO_CHOSEN_SERVER_SCREEN
             : SERVER_MAP_SCREEN;
}

}  // namespace

TitleScreen::TitleScreen(TitleScreenParts parts,
                         std::unique_ptr<IInterfaceCanvas> canvas)
    : canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      settings_(&parts.settings.get()),
      navigator_(&parts.navigator.get()),
      sounds_(&parts.sounds.get()),
      playStart_(parts.playStart),
      builtSize_(settings_->textSize),
      notice_(parts.notice),
      menu_(buildMenu()),
      squadron_(parts.motion, parts.seed, parts.squadronStart) {
  repaint();
}

void TitleScreen::handleEvent(const sf::Event& event) {
  menu_.handleEvent(event, playfieldPointAt_);
}

void TitleScreen::update(engine::Duration elapsed) {
  followTextSize();
  menu_.advance(elapsed);
  squadron_.advance(elapsed);
  askForNotice();
  repaint();
}

void TitleScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool TitleScreen::hidesScreensBelow() const { return true; }

void TitleScreen::followTextSize() {
  if (settings_->textSize == builtSize_) {
    return;
  }
  builtSize_ = settings_->textSize;
  menu_ = buildMenu();
}

Menu TitleScreen::buildMenu() const {
  const ScreenId playScreen = playScreenOf(playStart_);
  IScreenNavigator& navigator = *navigator_;
  return MenuBuilder(MenuBuilder::Frame::Page, sounds_,
                     textMetricsOf(settings_->textSize))
      .version(GAME_VERSION_TEXT)
      .logo(LOGO_ID, LOGO_SIZE)
      .subtitle(TITLE_SUBTITLE)
      .button({.label = PLAY_LABEL, .description = PLAY_DESCRIPTION},
              [&navigator, playScreen] { navigator.push(playScreen); })
      .button(
          {.label = LEADERBOARD_LABEL, .description = LEADERBOARD_DESCRIPTION},
          [&navigator] { navigator.push(LEADERBOARD_SCREEN); })
      .button({.label = SETTINGS_LABEL, .description = SETTINGS_DESCRIPTION},
              [&navigator] { navigator.push(OPTIONS_SCREEN); })
      .cancelButton({.label = QUIT_LABEL, .description = QUIT_DESCRIPTION},
                    [&navigator] { navigator.push(QUIT_CONFIRMATION_SCREEN); })
      .illustration(SQUADRON_BOX_SIZE)
      .footer(COPYRIGHT_TEXT)
      .build();
}

void TitleScreen::askForNotice() {
  if (notice_ == nullptr) {
    return;
  }
  if (!updateAsked_ && notice_->updateAvailable()) {
    updateAsked_ = true;
    navigator_->push(UPDATE_SCREEN);
  } else if (!notesAsked_ && notice_->notesWaiting()) {
    notesAsked_ = true;
    navigator_->push(WHATS_NEW_SCREEN);
  }
}

void TitleScreen::repaint() {
  canvas_->clear();
  menu_.paint(*canvas_);
  const std::optional<sf::IntRect> squadronBox = menu_.illustrationArea();
  if (squadronBox.has_value()) {
    squadron_.paint(*canvas_, squadronBox->position,
                    playerPaletteOf(*settings_));
  }
  paintKopecksBalance(*canvas_, KOPECKS_BALANCE_TOP_RIGHT, settings_->kopecks);
  canvas_->display();
}

}  // namespace rtype::client
