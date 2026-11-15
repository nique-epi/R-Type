#include "MenuPainting.hpp"
#include "Menu.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "TextMetrics.hpp"

InterfaceCanvasRecord paintingOf(const rtype::client::Menu& menu) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  menu.paint(canvas);
  return record;
}

InterfaceCanvasRecord paintingAt(const rtype::client::Menu& menu,
                                 rtype::client::TextMetrics metrics) {
  InterfaceCanvasRecord record;
  record.metrics = metrics;
  RecordingInterfaceCanvas canvas(record);
  menu.paint(canvas);
  return record;
}
