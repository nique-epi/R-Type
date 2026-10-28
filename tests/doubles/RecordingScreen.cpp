#include "RecordingScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <ostream>
#include <utility>
#include <vector>
#include "ScreenId.hpp"
#include "TimeConstants.hpp"

using rtype::client::ScreenId;

namespace {

const char* nameOf(ScreenId screen) {
  switch (screen) {
    case ScreenId::Title:
      return "title";
    case ScreenId::Game:
      return "game";
    case ScreenId::Options:
      return "options";
    case ScreenId::GameOptions:
      return "game options";
    case ScreenId::QuitConfirmation:
      return "quit confirmation";
    case ScreenId::Connection:
      return "connection";
    case ScreenId::Lobby:
      return "lobby";
    case ScreenId::Intro:
      return "intro";
    case ScreenId::TitleAfterIntro:
      return "title after the intro";
    case ScreenId::End:
      return "end";
    case ScreenId::LobbyAfterGame:
      return "lobby after a game";
    case ScreenId::Lost:
      return "lost";
  }
  return "unknown screen";
}

const char* nameOf(ScreenCall call) {
  switch (call) {
    case ScreenCall::Built:
      return "built";
    case ScreenCall::HandleEvent:
      return "handleEvent";
    case ScreenCall::Update:
      return "update";
    case ScreenCall::Draw:
      return "draw";
    case ScreenCall::Destroyed:
      return "destroyed";
  }
  return "unknown call";
}

}  // namespace

void PrintTo(const JournalEntry& entry, std::ostream* output) {
  *output << nameOf(entry.screen) << ' ' << nameOf(entry.call);
}

RecordingScreen::RecordingScreen(ScreenId screen,
                                 std::vector<JournalEntry>& journal)
    : screen_(screen), journal_(&journal) {
  write(ScreenCall::Built);
}

RecordingScreen::~RecordingScreen() {
  try {
    write(ScreenCall::Destroyed);
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

void RecordingScreen::runOnEvent(std::function<void()> action) {
  onEvent_ = std::move(action);
}

void RecordingScreen::setHidesScreensBelow(bool hides) {
  hidesScreensBelow_ = hides;
}

void RecordingScreen::handleEvent([[maybe_unused]] const sf::Event& event) {
  write(ScreenCall::HandleEvent);
  if (onEvent_ != nullptr) {
    onEvent_();
  }
}

void RecordingScreen::update(rtype::engine::Duration elapsed) {
  lastElapsed_ = elapsed;
  write(ScreenCall::Update);
}

void RecordingScreen::draw([[maybe_unused]] sf::RenderTarget& target) const {
  write(ScreenCall::Draw);
}

bool RecordingScreen::hidesScreensBelow() const { return hidesScreensBelow_; }

rtype::engine::Duration RecordingScreen::lastElapsed() const {
  return lastElapsed_;
}

void RecordingScreen::write(ScreenCall call) const {
  journal_->push_back(JournalEntry{.screen = screen_, .call = call});
}
