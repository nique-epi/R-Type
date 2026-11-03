#include "LeaderboardFilterDialogFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "KeyPress.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardFilterDialog.hpp"
#include "PointerEvents.hpp"
#include "WidgetConstants.hpp"

using rtype::client::LeaderboardFilterChoice;
using rtype::client::LeaderboardFilterDialog;
using rtype::client::PRESS_DURATION;

LeaderboardFilterDialogFixture::LeaderboardFilterDialogFixture() {
  dialog_.open({},
               [this](LeaderboardFilterChoice choice) { applied_ = choice; });
}

void LeaderboardFilterDialogFixture::choose(sf::Keyboard::Key key) {
  dialog_.handleEvent(keyPress(key), samePlayfieldPoint);
  dialog_.handleEvent(keyRelease(key), samePlayfieldPoint);
  dialog_.advance(PRESS_DURATION);
}

const LeaderboardFilterDialog& LeaderboardFilterDialogFixture::dialog() const {
  return dialog_;
}

const std::optional<LeaderboardFilterChoice>&
LeaderboardFilterDialogFixture::applied() const {
  return applied_;
}
