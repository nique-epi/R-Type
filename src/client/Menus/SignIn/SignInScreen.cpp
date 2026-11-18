#include "SignInScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <optional>
#include <utility>
#include "DeviceLinkPhase.hpp"
#include "DeviceLinkReading.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "SignInLines.hpp"
#include "SignInReturn.hpp"
#include "SignInScreenConstants.hpp"
#include "SignInScreenParts.hpp"
#include "SignInText.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

SignInScreen::SignInScreen(SignInScreenParts parts,
                           std::unique_ptr<IInterfaceCanvas> canvas)
    : parts_(std::move(parts)),
      canvas_(std::move(canvas)),
      menu_(MenuBuilder(MenuBuilder::Frame::Dialog).build()),
      ask_(parts_.deviceLinks.get().askLink()) {
  show(signInLinesOf(std::nullopt, parts_.now(), false));
  repaint();
}

void SignInScreen::handleEvent(const sf::Event& event) {
  menu_.handleEvent(event, parts_.playfieldPointAt);
}

void SignInScreen::update(engine::Duration elapsed) {
  followLink();
  std::optional<DeviceLinkReading> reading = parts_.deviceLinks.get().reading();
  if (reading.has_value() && reading->ask != ask_) {
    reading.reset();
  }
  show(signInLinesOf(reading, parts_.now(),
                     parts_.account.get().signInRefusal().has_value()));
  menu_.advance(elapsed);
  repaint();
}

void SignInScreen::draw(sf::RenderTarget& target) const {
  canvas_->show(target);
}

bool SignInScreen::hidesScreensBelow() const { return false; }

void SignInScreen::followLink() {
  const std::optional<DeviceLinkReading> reading =
      parts_.deviceLinks.get().reading();
  if (!reading.has_value() || reading->ask != ask_) {
    return;
  }
  if (reading->phase == DeviceLinkPhase::Waiting && !pageOpened_) {
    pageOpened_ = true;
    static_cast<void>(parts_.openPage(reading->verificationUriComplete));
  }
  if (reading->phase == DeviceLinkPhase::SignedIn && !signInAsked_) {
    signInAsked_ = true;
    parts_.account.get().signIn(reading->token);
  }
  if (signInAsked_ && parts_.account.get().signedIn()) {
    leaveSignedIn();
  }
}

void SignInScreen::leaveSignedIn() {
  if (left_) {
    return;
  }
  left_ = true;
  IScreenNavigator& navigator = parts_.navigator.get();
  navigator.pop();
  SignInReturn& signInReturn = parts_.signInReturn.get();
  if (signInReturn.closesAsking) {
    navigator.pop();
  }
  if (signInReturn.screen.has_value()) {
    navigator.push(*signInReturn.screen);
  }
  signInReturn = SignInReturn{};
}

void SignInScreen::show(const SignInLines& lines) {
  if (lines == shown_) {
    return;
  }
  shown_ = lines;
  MenuBuilder builder(MenuBuilder::Frame::Dialog, &parts_.sounds.get());
  builder.heading(lines.title).message(lines.line);
  if (lines.offersNewCode) {
    builder.button({.label = NEW_CODE_LABEL}, [this] { askNewCode(); });
  }
  menu_ =
      builder.cancelButton({.label = BACK_LABEL}, [this] { back(); }).build();
}

void SignInScreen::askNewCode() {
  ask_ = parts_.deviceLinks.get().askLink();
  pageOpened_ = false;
  signInAsked_ = false;
}

void SignInScreen::back() const {
  parts_.deviceLinks.get().askCancel();
  parts_.navigator.get().pop();
}

void SignInScreen::repaint() {
  canvas_->clear();
  menu_.paint(*canvas_);
  canvas_->display();
}

}  // namespace rtype::client
