#include "LoneGameParts.hpp"
#include <memory>
#include "GameScreenFixtures.hpp"
#include "GameScreenParts.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::GameScreenParts;

LoneGameParts::LoneGameParts() { seatThreePilots(connection); }

GameScreenParts LoneGameParts::parts() {
  return GameScreenParts{.navigator = navigator,
                         .settings = settings,
                         .inputs = inputs,
                         .entityMessages = entityMessages,
                         .connection = connection,
                         .sky = sky};
}

std::unique_ptr<RecordingInterfaceCanvas> LoneGameParts::statusCanvasDouble() {
  return std::make_unique<RecordingInterfaceCanvas>(statusCanvas);
}
