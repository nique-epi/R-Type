#include "MenuPainting.hpp"
#include "Menu.hpp"
#include "RecordingInterfaceCanvas.hpp"

InterfaceCanvasRecord paintingOf(const rtype::client::Menu& menu) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  menu.paint(canvas);
  return record;
}
