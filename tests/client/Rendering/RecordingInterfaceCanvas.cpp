#include "RecordingInterfaceCanvas.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <string>
#include <string_view>
#include "TextStyle.hpp"

RecordingInterfaceCanvas::RecordingInterfaceCanvas(
    InterfaceCanvasRecord& record)
    : record_(&record) {}

void RecordingInterfaceCanvas::clear() {
  record_->fills.clear();
  record_->writings.clear();
  record_->images.clear();
}

void RecordingInterfaceCanvas::fill(sf::IntRect area, sf::Color color) {
  record_->fills.push_back({.area = area, .color = color});
}

void RecordingInterfaceCanvas::write(std::string_view text, sf::Vector2i anchor,
                                     rtype::client::TextStyle style) {
  record_->writings.push_back(
      {.text = std::string(text), .anchor = anchor, .style = style});
}

void RecordingInterfaceCanvas::drawImage(std::string_view assetId,
                                         sf::IntRect source,
                                         sf::Vector2i topLeft,
                                         sf::IntRect visibleArea) {
  record_->images.push_back({.assetId = std::string(assetId),
                             .source = source,
                             .topLeft = topLeft,
                             .visibleArea = visibleArea});
}

void RecordingInterfaceCanvas::display() {}

void RecordingInterfaceCanvas::show(
    [[maybe_unused]] sf::RenderTarget& target) const {
  ++record_->showCount;
}

const RecordedWriting* writingOf(const InterfaceCanvasRecord& record,
                                 std::string_view text) {
  const auto found =
      std::ranges::find(record.writings, text, &RecordedWriting::text);
  return found == record.writings.end() ? nullptr : &*found;
}
