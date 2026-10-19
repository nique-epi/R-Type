#include "RecordingInterfaceCanvas.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
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
                                         sf::Vector2i topLeft, int scale,
                                         sf::IntRect visibleArea) {
  record_->images.push_back({.assetId = std::string(assetId),
                             .source = source,
                             .topLeft = topLeft,
                             .scale = scale,
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

std::optional<sf::Vector2i> anchorOf(const InterfaceCanvasRecord& record,
                                     std::string_view text) {
  const RecordedWriting* writing = writingOf(record, text);
  return writing == nullptr ? std::nullopt
                            : std::optional<sf::Vector2i>(writing->anchor);
}

std::set<std::pair<int, int>> litPixelsOf(const InterfaceCanvasRecord& record) {
  std::set<std::pair<int, int>> pixels;
  for (const RecordedFill& fill : record.fills) {
    for (int row = 0; row < fill.area.size.y; ++row) {
      for (int column = 0; column < fill.area.size.x; ++column) {
        pixels.emplace(fill.area.position.x + column,
                       fill.area.position.y + row);
      }
    }
  }
  return pixels;
}
