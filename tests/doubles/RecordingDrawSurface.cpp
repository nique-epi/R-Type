#include "RecordingDrawSurface.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "Position.hpp"
#include "SpriteDrawing.hpp"
#include "TextStyle.hpp"

void RecordingDrawSurface::draw(const rtype::client::SpriteDrawing& drawing) {
  drawn_.push_back({.assetId = std::string(drawing.assetId),
                    .frame = drawing.frame,
                    .center = drawing.center,
                    .tone = drawing.tone,
                    .outline = drawing.outline});
}

void RecordingDrawSurface::write(std::string_view text,
                                 rtype::game::Position center,
                                 rtype::client::TextStyle style) {
  written_.push_back(
      {.text = std::string(text), .center = center, .style = style});
}

const std::vector<DrawnImage>& RecordingDrawSurface::drawn() const {
  return drawn_;
}

const std::vector<WrittenLine>& RecordingDrawSurface::written() const {
  return written_;
}

std::vector<std::string> RecordingDrawSurface::assetIds() const {
  std::vector<std::string> ids;
  ids.reserve(drawn_.size());
  for (const DrawnImage& image : drawn_) {
    ids.push_back(image.assetId);
  }
  return ids;
}

void RecordingDrawSurface::forget() {
  drawn_.clear();
  written_.clear();
}
