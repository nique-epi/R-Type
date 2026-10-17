#include "RecordingDrawSurface.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "Position.hpp"
#include "SpriteFrame.hpp"

void RecordingDrawSurface::draw(std::string_view assetId,
                                rtype::game::SpriteFrame frame,
                                rtype::game::Position center) {
  drawn_.push_back(
      {.assetId = std::string(assetId), .frame = frame, .center = center});
}

const std::vector<DrawnImage>& RecordingDrawSurface::drawn() const {
  return drawn_;
}

std::vector<std::string> RecordingDrawSurface::assetIds() const {
  std::vector<std::string> ids;
  ids.reserve(drawn_.size());
  for (const DrawnImage& image : drawn_) {
    ids.push_back(image.assetId);
  }
  return ids;
}

void RecordingDrawSurface::forget() { drawn_.clear(); }
