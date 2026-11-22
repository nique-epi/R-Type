#include "TitleScreen.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <memory>
#include <optional>
#include <utility>
#include "ClientVersionConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlayStart.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SignInReturn.hpp"
#include "SignInScreenIdConstants.hpp"
#include "SquadronConstants.hpp"
#include "TimeConstants.hpp"
#include "TitleAccountChipsParts.hpp"
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
      account_(&parts.account.get()),
      deviceLinks_(&parts.deviceLinks.get()),
      signInReturn_(&parts.signInReturn.get()),
      notice_(parts.notice),
      sounds_(&parts.sounds.get()),
      playStart_(parts.playStart),
      builtIn_(settings_->language),
      menu_(buildMenu(std::nullopt)),
      accountChips_(TitleAccountChipsParts{.account = parts.account.get(),
                                           .wallet = parts.wallet,
                                           .sounds = parts.sounds,
                                           .signIn = [this] { signIn(); },
                                           .signOut = [this] { signOut(); },
                                           .language = settings_->language}),
      squadron_(parts.motion, parts.seed, parts.squadronStart) {
  repaint();
}

void TitleScreen::handleEvent(const sf::Event& event) {
  if (accountChips_.handleEvent(event, playfieldPointAt_)) {
    return;
  }
  menu_.handleEvent(event, playfieldPointAt_);
}

void TitleScreen::update(engine::Duration elapsed) {
  followLanguage();
  menu_.advance(elapsed);
  squadron_.advance(elapsed);
  askForNotice();
  repaint();
}

void TitleScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool TitleScreen::hidesScreensBelow() const { return true; }

Menu TitleScreen::buildMenu(std::optional<std::size_t> selected) const {
  IScreenNavigator& navigator = *navigator_;
  const Language language = settings_->language;
  const ScreenId playScreen = playScreenOf(playStart_);
  return MenuBuilder(MenuBuilder::Frame::Page, sounds_)
      .version(GAME_VERSION_TEXT)
      .logo(LOGO_ID, LOGO_SIZE)
      .subtitle(std::string(TITLE_SUBTITLE.in(language)))
      .button({.label = std::string(PLAY_LABEL.in(language)),
               .description = std::string(PLAY_DESCRIPTION.in(language))},
              [&navigator, playScreen] { navigator.push(playScreen); })
      .button(
          {.label = std::string(LEADERBOARD_LABEL.in(language)),
           .description = std::string(LEADERBOARD_DESCRIPTION.in(language))},
          [&navigator] { navigator.push(LEADERBOARD_SCREEN); })
      .button({.label = std::string(SETTINGS_LABEL.in(language)),
               .description = std::string(SETTINGS_DESCRIPTION.in(language))},
              [&navigator] { navigator.push(OPTIONS_SCREEN); })
      .cancelButton({.label = std::string(QUIT_LABEL.in(language)),
                     .description = std::string(QUIT_DESCRIPTION.in(language))},
                    [&navigator] { navigator.push(QUIT_CONFIRMATION_SCREEN); })
      .illustration(SQUADRON_BOX_SIZE)
      .footer(COPYRIGHT_TEXT)
      .build(selected);
}

void TitleScreen::followLanguage() {
  if (settings_->language == builtIn_) {
    return;
  }
  menu_ = buildMenu(menu_.selectedButton());
  accountChips_.followLanguage(settings_->language);
  builtIn_ = settings_->language;
}

void TitleScreen::signIn() {
  handleEvent(sf::Event::FocusLost{});
  *signInReturn_ = SignInReturn{};
  navigator_->push(SIGN_IN_SCREEN);
}

void TitleScreen::signOut() {
  deviceLinks_->askSignOut(account_->token());
  account_->signOut();
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
  accountChips_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
