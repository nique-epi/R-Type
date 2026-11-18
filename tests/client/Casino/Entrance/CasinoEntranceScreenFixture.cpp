#include "CasinoEntranceScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include "CasinoEntranceScreen.hpp"
#include "CasinoEntranceScreenParts.hpp"
#include "CasinoEntranceScreenTestConstants.hpp"
#include "CasinoRoom.hpp"
#include "CasinoRoomListing.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingCasinoChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RoulettePhase.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"
#include "Wallet.hpp"

using rtype::client::CasinoEntranceScreen;
using rtype::client::CasinoEntranceScreenParts;
using rtype::client::MotionPreference;
using rtype::client::testing::CHIPS_HELD;
using rtype::client::testing::DOORS_OPENING_TIME;
using rtype::client::testing::FREE_ROOM;
using rtype::client::testing::FULL_ROOM;
using rtype::client::testing::KOPECKS_HELD;
using rtype::client::testing::ROOM_SEATS;

namespace {

/** @returns The room every seat of which is taken. */
rtype::network::CasinoRoom fullRoom() {
  return rtype::network::CasinoRoom{
      .id = FULL_ROOM,
      .name = std::string{},
      .seats = ROOM_SEATS,
      .seated = ROOM_SEATS,
      .round = 0,
      .phase = rtype::network::RoulettePhase::Betting};
}

}  // namespace

CasinoEntranceScreenFixture::CasinoEntranceScreenFixture() {
  wallet_.balances =
      rtype::network::Wallet{.kopecks = KOPECKS_HELD, .chips = CHIPS_HELD};
  casino_.listedRooms = rtype::network::CasinoRoomListing{
      .rooms = {fullRoom(),
                rtype::network::CasinoRoom{
                    .id = FREE_ROOM,
                    .name = std::string{},
                    .seats = ROOM_SEATS,
                    .seated = 0,
                    .round = 0,
                    .phase = rtype::network::RoulettePhase::Betting}}};
  reopenWith(MotionPreference::Full);
}

void CasinoEntranceScreenFixture::reopenWith(MotionPreference motion) {
  screen_ = std::make_unique<CasinoEntranceScreen>(
      CasinoEntranceScreenParts{.navigator = navigator_,
                                .sounds = sounds_,
                                .casino = casino_,
                                .wallet = wallet_,
                                .playfieldPointAt = samePlayfieldPoint,
                                .motion = motion},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void CasinoEntranceScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CasinoEntranceScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->update(ONE_FRAME);
}

void CasinoEntranceScreenFixture::release(sf::Keyboard::Key key) {
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CasinoEntranceScreenFixture::wait(rtype::engine::Duration elapsed) {
  screen_->update(elapsed);
}

void CasinoEntranceScreenFixture::enterTheHall() {
  press(sf::Keyboard::Key::Enter);
  wait(DOORS_OPENING_TIME);
}

void CasinoEntranceScreenFixture::fillEveryRoom() {
  casino_.listedRooms =
      rtype::network::CasinoRoomListing{.rooms = {fullRoom()}};
  screen_->update(ONE_FRAME);
}

const RecordingNavigator& CasinoEntranceScreenFixture::navigator() const {
  return navigator_;
}

const RecordingCasinoChannel& CasinoEntranceScreenFixture::casino() const {
  return casino_;
}

const InterfaceCanvasRecord& CasinoEntranceScreenFixture::canvas() const {
  return canvas_;
}
