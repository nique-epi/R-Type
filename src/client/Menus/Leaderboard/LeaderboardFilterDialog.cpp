#include "LeaderboardFilterDialog.hpp"
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>
#include "ChoiceSelector.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardScreenConstants.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PlayfieldMapping.hpp"
#include "RowControl.hpp"
#include "TimeConstants.hpp"
#include "ValueBinding.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns A selector of these labels over @p value, which must outlive it. */
std::unique_ptr<RowControl> selectorOver(std::span<const char* const> labels,
                                         std::size_t& value) {
  return std::make_unique<ChoiceSelector>(
      std::vector<std::string>(labels.begin(), labels.end()),
      ValueBinding<std::size_t>{
          .read = [&value] { return value; },
          .write = [&value](std::size_t chosen) { value = chosen; }});
}

}  // namespace

LeaderboardFilterDialog::LeaderboardFilterDialog(ISoundPlayer& sounds)
    : sounds_(&sounds) {}

void LeaderboardFilterDialog::open(
    LeaderboardFilterChoice choice,
    const std::function<void(LeaderboardFilterChoice)>& apply) {
  draft_ = choice;
  dialog_ =
      MenuBuilder(MenuBuilder::Frame::Dialog, sounds_)
          .heading(FILTER_LABEL)
          .row({.label = DIFFICULTY_ROW_LABEL},
               selectorOver(LEADERBOARD_DIFFICULTY_LABELS, draft_.difficulty))
          .row({.label = PILOTS_ROW_LABEL},
               selectorOver(LEADERBOARD_PILOT_LABELS, draft_.pilots))
          .cancelButton({.label = BACK_LABEL}, [this] { answered_ = true; })
          .button({.label = APPLY_LABEL},
                  [this, apply] {
                    apply(draft_);
                    answered_ = true;
                  })
          .build();
}

bool LeaderboardFilterDialog::isOpen() const { return dialog_.has_value(); }

void LeaderboardFilterDialog::handleEvent(const sf::Event& event,
                                          const PlayfieldMapping& pointAt) {
  if (dialog_.has_value()) {
    dialog_->handleEvent(event, pointAt);
  }
  closeIfAnswered();
}

void LeaderboardFilterDialog::advance(engine::Duration elapsed) {
  if (dialog_.has_value()) {
    dialog_->advance(elapsed);
  }
  closeIfAnswered();
}

void LeaderboardFilterDialog::paint(IInterfaceCanvas& canvas) const {
  if (dialog_.has_value()) {
    dialog_->paint(canvas);
  }
}

void LeaderboardFilterDialog::closeIfAnswered() {
  if (answered_) {
    answered_ = false;
    dialog_.reset();
  }
}

}  // namespace rtype::client
