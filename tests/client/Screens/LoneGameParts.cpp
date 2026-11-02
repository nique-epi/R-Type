#include "LoneGameParts.hpp"
#include <memory>
#include "GameScreenFixtures.hpp"
#include "GameScreenParts.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::GameScreenParts;

LoneGameParts::LoneGameParts() { seatThreePilots(connection, waitingRoom); }

GameScreenParts LoneGameParts::parts() {
  return GameScreenParts{.navigator = navigator,
                         .settings = settings,
                         .inputs = inputs,
                         .entityMessages = entityMessages,
                         .connection = connection,
                         .waitingRoom = waitingRoom,
                         .sky = sky,
                         .sounds = sounds,
                         .result = result};
}

std::unique_ptr<RecordingInterfaceCanvas> LoneGameParts::statusCanvasDouble() {
  return std::make_unique<RecordingInterfaceCanvas>(statusCanvas);
}
