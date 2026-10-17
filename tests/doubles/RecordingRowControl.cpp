#include "RecordingRowControl.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <utility>
#include <vector>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"

namespace {

constexpr int RECORDING_CONTROL_WIDTH = 40;
constexpr sf::Color RECORDING_CONTROL_COLOR(1, 2, 3);

}  // namespace

RecordingRowControl::RecordingRowControl(
    RowControlRecord& record, std::vector<sf::Keyboard::Key> usedKeys,
    bool activatable)
    : record_(&record),
      usedKeys_(std::move(usedKeys)),
      activatable_(activatable) {}

int RecordingRowControl::width() const { return RECORDING_CONTROL_WIDTH; }

void RecordingRowControl::paint(
    rtype::client::IInterfaceCanvas& canvas, sf::IntRect area,
    [[maybe_unused]] rtype::client::BarState row) const {
  canvas.fill(area, RECORDING_CONTROL_COLOR);
}

bool RecordingRowControl::isActivatable() const { return activatable_; }

void RecordingRowControl::activate() { ++record_->activations; }

bool RecordingRowControl::holdsInput() const { return record_->holding; }

bool RecordingRowControl::keyPressed(const sf::Event::KeyPressed& key) {
  record_->keys.push_back(key.code);
  return record_->holding ||
         std::ranges::find(usedKeys_, key.code) != usedKeys_.end();
}

bool RecordingRowControl::pointerPressed([[maybe_unused]] sf::Vector2i pixel,
                                         [[maybe_unused]] sf::IntRect area) {
  ++record_->pointerPresses;
  return true;
}

void RecordingRowControl::cancelInput() {
  ++record_->cancels;
  record_->holding = false;
}
