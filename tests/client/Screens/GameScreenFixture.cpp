#include "GameScreenFixture.hpp"
#include <memory>
#include <vector>
#include "DrawJournal.hpp"
#include "FakeEntityMessageSource.hpp"
#include "GameScreen.hpp"
#include "GameScreenFixtures.hpp"
#include "GameScreenParts.hpp"
#include "IScreen.hpp"
#include "PlayerSettings.hpp"
#include "RecordingEntityLayer.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingScreen.hpp"
#include "RecordingSky.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"

using rtype::client::GameScreen;
using rtype::client::GameScreenParts;
using rtype::client::IScreen;
using rtype::client::PlayerSettings;
using rtype::client::ScreenId;
using rtype::client::ScreenStack;

GameScreenFixture::GameScreenFixture()
    : screens_([this](ScreenId screen) -> std::unique_ptr<IScreen> {
        if (screen == ScreenId::Game) {
          return std::make_unique<GameScreen>(
              GameScreenParts{.navigator = screens_,
                              .settings = settings_,
                              .inputs = inputs_,
                              .entityMessages = entityMessages_,
                              .connection = connection_,
                              .sky = sky_,
                              .sounds = sounds_,
                              .result = result_},
              std::make_unique<RecordingEntityLayer>(layerRecord_),
              std::make_unique<RecordingInterfaceCanvas>(statusCanvas_));
        }
        return std::make_unique<RecordingScreen>(screen, journal_);
      }) {}

void GameScreenFixture::SetUp() {
  seatThreePilots(connection_);
  layerRecord_.drawJournal = &drawJournal_;
  statusCanvas_.drawJournal = &drawJournal_;
  screens_.push(ScreenId::Game);
  screens_.applyPendingChanges();
}

ScreenStack& GameScreenFixture::screens() { return screens_; }

const std::vector<JournalEntry>& GameScreenFixture::journal() const {
  return journal_;
}

const EntityLayerRecord& GameScreenFixture::layerRecord() const {
  return layerRecord_;
}

PlayerSettings& GameScreenFixture::settings() { return settings_; }

const RecordingSky& GameScreenFixture::sky() const { return sky_; }

FakeEntityMessageSource& GameScreenFixture::entityMessages() {
  return entityMessages_;
}

const InterfaceCanvasRecord& GameScreenFixture::statusCanvas() const {
  return statusCanvas_;
}

const DrawJournal& GameScreenFixture::drawJournal() const {
  return drawJournal_;
}

const RecordingSoundPlayer& GameScreenFixture::sounds() const {
  return sounds_;
}
