#include "EndScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include "EndScreen.hpp"
#include "EndScreenParts.hpp"
#include "EndScreenTestConstants.hpp"
#include "FinalPilot.hpp"
#include "FlashPreference.hpp"
#include "InterfaceColorConstants.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::EndScreen;
using rtype::client::EndScreenParts;
using rtype::client::FinalPilot;
using rtype::client::MotionPreference;
using rtype::client::testing::ACE_NICKNAME;
using rtype::client::testing::ACE_NUMBER;
using rtype::client::testing::ACE_RANK;
using rtype::client::testing::ACE_SCORE;
using rtype::client::testing::END_SEED;
using rtype::client::testing::NOVA_NICKNAME;
using rtype::client::testing::NOVA_NUMBER;
using rtype::client::testing::NOVA_RANK;
using rtype::client::testing::NOVA_SCORE;
using rtype::client::testing::STARDUST_NICKNAME;
using rtype::client::testing::STARDUST_NUMBER;
using rtype::client::testing::STARDUST_RANK;
using rtype::client::testing::STARDUST_SCORE;

EndScreenFixture::EndScreenFixture()
    : result_{.pilots = {FinalPilot{.playerNumber = STARDUST_NUMBER,
                                    .nickname = std::string{STARDUST_NICKNAME},
                                    .score = STARDUST_SCORE,
                                    .isLocalPlayer = false,
                                    .rank = STARDUST_RANK},
                         FinalPilot{.playerNumber = ACE_NUMBER,
                                    .nickname = std::string{ACE_NICKNAME},
                                    .score = ACE_SCORE,
                                    .isLocalPlayer = true,
                                    .rank = ACE_RANK},
                         FinalPilot{.playerNumber = NOVA_NUMBER,
                                    .nickname = std::string{NOVA_NICKNAME},
                                    .score = NOVA_SCORE,
                                    .isLocalPlayer = false,
                                    .rank = NOVA_RANK}}} {
  rebuildWith(MotionPreference::Full);
}

void EndScreenFixture::rebuildWith(MotionPreference motion) {
  screen_.reset();
  screen_ = std::make_unique<EndScreen>(
      EndScreenParts{.navigator = navigator_,
                     .sounds = sounds_,
                     .connection = connection_,
                     .result = result_,
                     .previousTaunt = previousTaunt_,
                     .playfieldPointAt = samePlayfieldPoint,
                     .seed = END_SEED,
                     .motion = motion,
                     .flashes = rtype::client::FlashPreference::Full,
                     .palette = rtype::client::PLAYER_COLORS},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void EndScreenFixture::tap(sf::Keyboard::Key key) {
  screen().handleEvent(keyPress(key));
  screen().handleEvent(keyRelease(key));
}

void EndScreenFixture::click(sf::Vector2i interfacePixel) {
  screen().handleEvent(leftPress(interfacePixel * INTERFACE_PIXEL));
}

void EndScreenFixture::advance(rtype::engine::Duration duration) {
  while (duration > rtype::engine::Duration::zero()) {
    const rtype::engine::Duration step = std::min(duration, ONE_FRAME);
    screen().update(step);
    duration -= step;
  }
}

EndScreen& EndScreenFixture::screen() { return *screen_; }

const RecordingNavigator& EndScreenFixture::navigator() const {
  return navigator_;
}

const RecordingServerConnection& EndScreenFixture::connection() const {
  return connection_;
}

const InterfaceCanvasRecord& EndScreenFixture::canvas() const {
  return canvas_;
}

const RecordingSoundPlayer& EndScreenFixture::sounds() const { return sounds_; }

std::optional<std::size_t> EndScreenFixture::previousTaunt() const {
  return previousTaunt_;
}
