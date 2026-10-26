#pragma once

#include <memory>
#include "FakeEntityMessageSource.hpp"
#include "GameScreenParts.hpp"
#include "PlayerSettings.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSky.hpp"
#include "RecordingSoundPlayer.hpp"

/** @brief What a game screen alone is built from, every part a double, the
 * connection accepted as player 2 of a waiting room of three. */
struct LoneGameParts {
  LoneGameParts();

  RecordingNavigator navigator;
  rtype::client::PlayerSettings settings;
  RecordingPlayerInputLink inputs;
  FakeEntityMessageSource entityMessages;
  RecordingServerConnection connection;
  InterfaceCanvasRecord statusCanvas;
  RecordingSky sky;
  RecordingSoundPlayer sounds;

  /** @returns The parts of a game screen, borrowing these doubles. */
  [[nodiscard]] rtype::client::GameScreenParts parts();

  /** @returns A status canvas recording into statusCanvas. */
  [[nodiscard]] std::unique_ptr<RecordingInterfaceCanvas> statusCanvasDouble();
};
