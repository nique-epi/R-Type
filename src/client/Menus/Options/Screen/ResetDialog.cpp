#include "ResetDialog.hpp"
#include <SFML/Window/Event.hpp>
#include <functional>
#include <string>
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PlayfieldMapping.hpp"
#include "ScreenConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ResetDialog::ResetDialog(ISoundPlayer& sounds) : sounds_(&sounds) {}

void ResetDialog::ask(const std::string& question,
                      const std::function<void()>& reset, Language language) {
  dialog_ = MenuBuilder(MenuBuilder::Frame::Dialog, sounds_)
                .heading(std::string(CONFIRM_HEADING.in(language)))
                .message(question)
                .cancelButton({.label = std::string(NO_LABEL.in(language))},
                              [this] { answered_ = true; })
                .button({.label = std::string(YES_LABEL.in(language))},
                        [this, reset] {
                          reset();
                          answered_ = true;
                        })
                .build();
}

bool ResetDialog::isOpen() const { return dialog_.has_value(); }

void ResetDialog::handleEvent(const sf::Event& event,
                              const PlayfieldMapping& pointAt) {
  if (dialog_.has_value()) {
    dialog_->handleEvent(event, pointAt);
  }
  closeIfAnswered();
}

void ResetDialog::advance(engine::Duration elapsed) {
  if (dialog_.has_value()) {
    dialog_->advance(elapsed);
  }
  closeIfAnswered();
}

void ResetDialog::paint(IInterfaceCanvas& canvas) const {
  if (dialog_.has_value()) {
    dialog_->paint(canvas);
  }
}

void ResetDialog::closeIfAnswered() {
  if (answered_) {
    answered_ = false;
    dialog_.reset();
  }
}

}  // namespace rtype::client
