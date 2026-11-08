#include "WarningBannerFixtures.hpp"
#include <vector>
#include "RecordingInterfaceCanvas.hpp"
#include "WarningBanner.hpp"

InterfaceCanvasRecord warningPaintingOf(
    const rtype::client::WarningBannerLook& look) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintWarningBanner(canvas, look);
  return record;
}

std::vector<RecordedWriting> clippedWritingsOf(
    const InterfaceCanvasRecord& record) {
  std::vector<RecordedWriting> clipped;
  for (const RecordedWriting& writing : record.writings) {
    if (writing.style.clip.has_value()) {
      clipped.push_back(writing);
    }
  }
  return clipped;
}
