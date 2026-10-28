#include "NoSignalCardFixtures.hpp"
#include "NoSignalCard.hpp"
#include "NoSignalLook.hpp"
#include "RecordingInterfaceCanvas.hpp"

InterfaceCanvasRecord cardPaintingOf(const rtype::client::NoSignalLook& look) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintNoSignalCard(canvas, look);
  return record;
}
