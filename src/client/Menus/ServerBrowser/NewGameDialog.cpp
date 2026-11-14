#include "NewGameDialog.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ChoiceSelector.hpp"
#include "ChosenGame.hpp"
#include "ChosenGameConnection.hpp"
#include "ConnectionRequest.hpp"
#include "GameRequest.hpp"
#include "GameRuleConstants.hpp"
#include "GameRules.hpp"
#include "GameRulesLine.hpp"
#include "IScreenNavigator.hpp"
#include "Menu.hpp"
#include "MenuBuilder.hpp"
#include "NewGameDialogConstants.hpp"
#include "NewGameMenuParts.hpp"
#include "RowControl.hpp"
#include "ServerChoice.hpp"
#include "Switch.hpp"
#include "ValueBinding.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The rules the dialog edits and the difficulties it offers, shared
 * by the bindings of its rows and by CREATE, so it lives as long as the menu.
 */
struct NewGameDraft {
  std::vector<std::string> difficulties;
  network::GameRules rules;
};

/** @returns The draft a dialog opens on: the default rules, played at the
 * first of @p difficulties when DEFAULT_DIFFICULTY is not among them, and
 * DEFAULT_DIFFICULTY alone offered when @p difficulties is empty. */
std::shared_ptr<NewGameDraft> startingDraft(
    std::vector<std::string> difficulties) {
  if (difficulties.empty()) {
    difficulties.emplace_back(identifiers::DEFAULT_DIFFICULTY);
  }
  network::GameRules rules = network::defaultGameRules();
  if (std::ranges::find(difficulties, rules.difficulty) == difficulties.end()) {
    rules.difficulty = difficulties.front();
  }
  return std::make_shared<NewGameDraft>(
      NewGameDraft{.difficulties = std::move(difficulties), .rules = rules});
}

/** @returns The selector of the difficulty of @p draft, its names in
 * capitals. */
std::unique_ptr<RowControl> difficultySelector(
    const std::shared_ptr<NewGameDraft>& draft) {
  std::vector<std::string> labels;
  labels.reserve(draft->difficulties.size());
  std::ranges::transform(draft->difficulties, std::back_inserter(labels),
                         upperCased);
  return std::make_unique<ChoiceSelector>(
      std::move(labels),
      ValueBinding<std::size_t>{
          .read =
              [draft] {
                return static_cast<std::size_t>(
                    std::ranges::find(draft->difficulties,
                                      draft->rules.difficulty) -
                    draft->difficulties.begin());
              },
          .write =
              [draft](std::size_t chosen) {
                draft->rules.difficulty = draft->difficulties.at(chosen);
              }});
}

/** @returns The selector of the lives of @p draft, LOWEST_LIVES to
 * HIGHEST_LIVES. */
std::unique_ptr<RowControl> livesSelector(
    const std::shared_ptr<NewGameDraft>& draft) {
  std::vector<std::string> labels;
  for (int lives = identifiers::LOWEST_LIVES;
       std::cmp_less_equal(lives, identifiers::HIGHEST_LIVES); ++lives) {
    labels.push_back(std::to_string(lives));
  }
  return std::make_unique<ChoiceSelector>(
      std::move(labels),
      ValueBinding<std::size_t>{
          .read =
              [draft] {
                return static_cast<std::size_t>(draft->rules.lives -
                                                identifiers::LOWEST_LIVES);
              },
          .write =
              [draft](std::size_t chosen) {
                draft->rules.lives = static_cast<std::uint8_t>(
                    identifiers::LOWEST_LIVES + chosen);
              }});
}

/** @returns The switch of the friendly fire of @p draft. */
std::unique_ptr<RowControl> friendlyFireSwitch(
    const std::shared_ptr<NewGameDraft>& draft) {
  return std::make_unique<Switch>(
      ValueBinding<bool>{.read = [draft] { return draft->rules.friendlyFire; },
                         .write =
                             [draft](bool friendlyFire) {
                               draft->rules.friendlyFire = friendlyFire;
                             }});
}

}  // namespace

Menu newGameMenu(NewGameMenuParts parts) {
  const std::shared_ptr<NewGameDraft> draft =
      startingDraft(std::move(parts.difficulties));
  ServerChoice& choice = parts.choice.get();
  IScreenNavigator& navigator = parts.navigator.get();
  return MenuBuilder(MenuBuilder::Frame::Dialog, &parts.sounds.get(),
                     parts.metrics)
      .heading(NEW_GAME_HEADING)
      .row({.label = DIFFICULTY_ROW_LABEL}, difficultySelector(draft))
      .row({.label = NEW_GAME_LIVES_LABEL}, livesSelector(draft))
      .row({.label = NEW_GAME_FRIENDLY_FIRE_LABEL}, friendlyFireSwitch(draft))
      .cancelButton({.label = BACK_LABEL}, [&navigator] { navigator.pop(); })
      .button({.label = CREATE_LABEL},
              [draft, server = parts.server, &choice, &navigator] {
                navigator.pop();
                openConnectionTo(
                    choice,
                    ChosenGame{
                        .server = server,
                        .request = GameRequest{.game = network::NEW_GAME_CHOICE,
                                               .rules = draft->rules},
                        .otherServers = {}},
                    navigator);
              })
      .build();
}

}  // namespace rtype::client
