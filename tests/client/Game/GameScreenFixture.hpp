#pragma once

#include <gtest/gtest.h>
#include <vector>
#include "DrawJournal.hpp"
#include "FakeEntityMessageSource.hpp"
#include "GameResult.hpp"
#include "PlayerSettings.hpp"
#include "RecordingEntityLayer.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingPlayerInputLink.hpp"
#include "RecordingScreen.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSky.hpp"
#include "RecordingSoundPlayer.hpp"
#include "RecordingWaitingRoom.hpp"
#include "ScreenStack.hpp"

/** @brief The real game screen alone in a stack whose other screens are
 * recording doubles, with a recording entity layer, a recording status
 * canvas, a source of entity messages the test fills, and a connection
 * accepted as player 2 of a waiting room of three. */
class GameScreenFixture : public ::testing::Test {
 public:
  /** @brief Builds the stack: the game screen from the doubles of the
   * fixture, every other screen as a RecordingScreen writing in journal(). */
  GameScreenFixture();

 protected:
  void SetUp() override;

  rtype::client::ScreenStack& screens();

  [[nodiscard]] const std::vector<JournalEntry>& journal() const;

  [[nodiscard]] const EntityLayerRecord& layerRecord() const;

  rtype::client::PlayerSettings& settings();

  [[nodiscard]] const RecordingSky& sky() const;

  FakeEntityMessageSource& entityMessages();

  [[nodiscard]] const InterfaceCanvasRecord& statusCanvas() const;

  [[nodiscard]] const DrawJournal& drawJournal() const;

  [[nodiscard]] const RecordingSoundPlayer& sounds() const;

 private:
  std::vector<JournalEntry> journal_;
  EntityLayerRecord layerRecord_;
  rtype::client::PlayerSettings settings_;
  RecordingPlayerInputLink inputs_;
  FakeEntityMessageSource entityMessages_;
  RecordingServerConnection connection_;
  RecordingWaitingRoom waitingRoom_;
  InterfaceCanvasRecord statusCanvas_;
  DrawJournal drawJournal_;
  RecordingSky sky_;
  RecordingSoundPlayer sounds_;
  rtype::client::GameResult result_;
  rtype::client::ScreenStack screens_;
};
