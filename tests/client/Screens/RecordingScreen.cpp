#include "RecordingScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include "ScreenStack.hpp"

namespace {

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
  *output << entry.screen << ' ' << nameOf(entry.call);
}

RecordingScreen::RecordingScreen(std::string name,
                                 std::vector<JournalEntry>& journal)
    : name_(std::move(name)), journal_(&journal) {
  write(ScreenCall::Built);
}

RecordingScreen::~RecordingScreen() { write(ScreenCall::Destroyed); }

void RecordingScreen::runOnEvent(std::function<void()> action) {
  onEvent_ = std::move(action);
}

void RecordingScreen::handleEvent([[maybe_unused]] const sf::Event& event) {
  write(ScreenCall::HandleEvent);
  if (onEvent_ != nullptr) {
    onEvent_();
  }
}

void RecordingScreen::update() { write(ScreenCall::Update); }

void RecordingScreen::draw([[maybe_unused]] sf::RenderTarget& target) const {
  write(ScreenCall::Draw);
}

void RecordingScreen::write(ScreenCall call) const {
  journal_->push_back(JournalEntry{.screen = name_, .call = call});
}

rtype::client::ScreenFactory recordingScreen(
    std::string name, std::vector<JournalEntry>& journal) {
  return [name = std::move(name), &journal] {
    return std::make_unique<RecordingScreen>(name, journal);
  };
}
