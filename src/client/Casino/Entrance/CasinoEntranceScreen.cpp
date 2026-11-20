#include "CasinoEntranceScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <memory>
#include <utility>
#include "ButlerMotion.hpp"
#include "CasinoBarChip.hpp"
#include "CasinoEntranceConstants.hpp"
#include "CasinoEntrancePainting.hpp"
#include "CasinoEntranceScreenParts.hpp"
#include "CasinoScreenIdConstants.hpp"
#include "EntranceStage.hpp"
#include "EntranceView.hpp"
#include "IInterfaceCanvas.hpp"
#include "KeyChip.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p event skips the opening of the doors: a key pressed,
 * or the left button. */
bool skipsOpening(const sf::Event& event) {
  const auto* click = event.getIf<sf::Event::MouseButtonPressed>();
  return event.is<sf::Event::KeyPressed>() ||
         (click != nullptr && click->button == sf::Mouse::Button::Left);
}

}  // namespace

CasinoEntranceScreen::CasinoEntranceScreen(
    CasinoEntranceScreenParts parts, std::unique_ptr<IInterfaceCanvas> canvas)
    : navigator_(&parts.navigator.get()),
      canvas_(std::move(canvas)),
      playfieldPointAt_(std::move(parts.playfieldPointAt)),
      motion_(parts.motion),
      lights_(std::move(parts.lights)),
      enterChip_(chipOf(
          ENTER_BAR, [this] { enter(); }, parts.sounds)),
      doorsBackChip_(
          chipOf(DOORS_BACK_BAR, [this] { navigator_->pop(); }, parts.sounds)) {
  repaint();
}

void CasinoEntranceScreen::handleEvent(const sf::Event& event) {
  if (stage_ == EntranceStage::Opening) {
    if (skipsOpening(event)) {
      openTheHall();
    }
    return;
  }
  if (!enterChip_.handleEvent(event, playfieldPointAt_)) {
    static_cast<void>(doorsBackChip_.handleEvent(event, playfieldPointAt_));
  }
}

void CasinoEntranceScreen::update(engine::Duration elapsed) {
  sinceStage_ += elapsed;
  since_ += elapsed;
  if (stage_ == EntranceStage::Opening && sinceStage_ >= OPENING_DURATION) {
    openTheHall();
  }
  repaint();
}

void CasinoEntranceScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool CasinoEntranceScreen::hidesScreensBelow() const { return true; }

void CasinoEntranceScreen::enter() {
  if (motion_ == MotionPreference::Full) {
    reach(EntranceStage::Opening);
  } else {
    openTheHall();
  }
}

void CasinoEntranceScreen::openTheHall() {
  for (KeyChip* chip : {&enterChip_, &doorsBackChip_}) {
    static_cast<void>(
        chip->handleEvent(sf::Event::FocusLost{}, playfieldPointAt_));
  }
  reach(EntranceStage::Doors);
  navigator_->push(CASINO_HALL_SCREEN);
}

void CasinoEntranceScreen::reach(EntranceStage stage) {
  stage_ = stage;
  sinceStage_ = engine::Duration::zero();
}

void CasinoEntranceScreen::repaint() {
  const EntranceView view{.front = doorsPictureAt(stage_, sinceStage_),
                          .butler = butlerFrameAt(stage_, sinceStage_, motion_),
                          .line = WELCOME_LINE};
  canvas_->clear();
  paintFront(*canvas_, view);
  if (motion_ == MotionPreference::Full) {
    lights_.paint(*canvas_, since_);
  }
  paintGreeter(*canvas_, view);
  if (stage_ == EntranceStage::Doors) {
    enterChip_.paint(*canvas_);
    doorsBackChip_.paint(*canvas_);
  }
  canvas_->display();
}

}  // namespace rtype::client
