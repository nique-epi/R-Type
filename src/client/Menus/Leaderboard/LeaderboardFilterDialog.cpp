#include "LeaderboardFilterDialog.hpp"
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ChoiceSelector.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "Language.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardScreenConstants.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "PlayfieldMapping.hpp"
#include "RowControl.hpp"
#include "TimeConstants.hpp"
#include "Translation.hpp"
#include "ValueBinding.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns A selector of these labels, written in @p language, over
 * @p value, which must outlive it. */
std::unique_ptr<RowControl> selectorOver(std::span<const Translation> labels,
                                         std::size_t& value,
                                         Language language) {
  std::vector<std::string> texts;
  texts.reserve(labels.size());
  for (const Translation& label : labels) {
    texts.emplace_back(label.in(language));
  }
  return std::make_unique<ChoiceSelector>(
      std::move(texts),
      ValueBinding<std::size_t>{
          .read = [&value] { return value; },
          .write = [&value](std::size_t chosen) { value = chosen; }});
}

}  // namespace

LeaderboardFilterDialog::LeaderboardFilterDialog(ISoundPlayer& sounds)
    : sounds_(&sounds) {}

void LeaderboardFilterDialog::open(
    LeaderboardFilterChoice choice,
    const std::function<void(LeaderboardFilterChoice)>& apply,
    Language language) {
  draft_ = choice;
  dialog_ =
      MenuBuilder(MenuBuilder::Frame::Dialog, sounds_)
          .heading(std::string(FILTER_LABEL.in(language)))
          .row({.label = std::string(DIFFICULTY_ROW_LABEL.in(language))},
               selectorOver(LEADERBOARD_DIFFICULTY_LABELS, draft_.difficulty,
                            language))
          .row({.label = std::string(PILOTS_ROW_LABEL.in(language))},
               selectorOver(LEADERBOARD_PILOT_LABELS, draft_.pilots, language))
          .cancelButton({.label = std::string(BACK_LABEL.in(language))},
                        [this] { answered_ = true; })
          .button({.label = std::string(APPLY_LABEL.in(language))},
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
