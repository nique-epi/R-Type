#include "GlowingCallPaintingFixtures.hpp"
#include "GlowingCall.hpp"
#include "GlowingCallPainting.hpp"
#include "RecordingInterfaceCanvas.hpp"

InterfaceCanvasRecord callPaintingOf(const rtype::client::GlowingCall& call) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  rtype::client::paintGlowingCall(canvas, call);
  return record;
}
