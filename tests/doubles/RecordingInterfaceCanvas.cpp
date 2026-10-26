#include "RecordingInterfaceCanvas.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Utf.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "DrawJournal.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "RecordingInterfaceCanvasConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"

RecordingInterfaceCanvas::RecordingInterfaceCanvas(
    InterfaceCanvasRecord& record)
    : record_(&record) {}

void RecordingInterfaceCanvas::clear() {
  record_->fills.clear();
  record_->writings.clear();
  record_->images.clear();
  record_->smoothImages.clear();
}

void RecordingInterfaceCanvas::fill(sf::IntRect area, sf::Color color) {
  record_->fills.push_back({.area = area, .color = color});
}

void RecordingInterfaceCanvas::write(std::string_view text, sf::Vector2i anchor,
                                     rtype::client::TextStyle style) {
  record_->writings.push_back(
      {.text = std::string(text), .anchor = anchor, .style = style});
}

void RecordingInterfaceCanvas::drawImage(
    rtype::client::TextureArea image, rtype::client::ImagePlacement placement,
    sf::IntRect visibleArea) {
  record_->images.push_back({.assetId = std::string(image.assetId),
                             .source = image.rectangle,
                             .topLeft = placement.topLeft,
                             .scale = placement.scale,
                             .visibleArea = visibleArea,
                             .tint = placement.tint});
}

void RecordingInterfaceCanvas::drawSmoothImage(std::string_view assetId,
                                               sf::IntRect area,
                                               sf::Color tint) {
  record_->smoothImages.push_back(
      {.assetId = std::string(assetId), .area = area, .tint = tint});
}

int RecordingInterfaceCanvas::textWidth(std::string_view text,
                                        rtype::client::TextFont font) const {
  const int advance = font == rtype::client::TextFont::Regular
                          ? rtype::client::REGULAR_FONT_ADVANCE
                          : RECORDED_SMALL_ADVANCE;
  return static_cast<int>(sf::Utf8::count(text.begin(), text.end())) * advance;
}

void RecordingInterfaceCanvas::display() { ++record_->displayCount; }

void RecordingInterfaceCanvas::show(
    [[maybe_unused]] sf::RenderTarget& target) const {
  ++record_->showCount;
  if (record_->drawJournal != nullptr) {
    record_->drawJournal->laid.emplace_back(INTERFACE_CANVAS_LAID);
  }
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

std::vector<sf::Vector2i> untintedImagesOf(const InterfaceCanvasRecord& record,
                                           std::string_view assetId,
                                           sf::Vector2i size) {
  std::vector<sf::Vector2i> places;
  for (const RecordedImage& image : record.images) {
    if (image.assetId == assetId && image.source.size == size &&
        image.tint == sf::Color::White) {
      places.push_back(image.topLeft);
    }
  }
  return places;
}
