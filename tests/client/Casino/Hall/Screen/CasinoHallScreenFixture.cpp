#include "CasinoHallScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include "CasinoHallScreen.hpp"
#include "CasinoHallScreenParts.hpp"
#include "CasinoHallScreenTestConstants.hpp"
#include "CasinoRoom.hpp"
#include "CasinoRoomListing.hpp"
#include "HallPicture.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingCasinoChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RoulettePhase.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::CasinoHallScreen;
using rtype::client::CasinoHallScreenParts;
using rtype::client::HallPicture;
using rtype::client::MotionPreference;
using rtype::client::testing::HALL_FREE_ROOM;
using rtype::client::testing::HALL_FULL_ROOM;
using rtype::client::testing::HALL_ROOM_SEATS;

namespace {

/** @returns A room of the master, @p seated of its seats taken. */
rtype::network::CasinoRoom roomOf(std::uint8_t id, std::uint8_t seated) {
  return rtype::network::CasinoRoom{
      .id = id,
      .name = std::string{},
      .seats = HALL_ROOM_SEATS,
      .seated = seated,
      .round = 0,
      .phase = rtype::network::RoulettePhase::Betting};
}

}  // namespace

CasinoHallScreenFixture::CasinoHallScreenFixture() {
  casino_.listedRooms = rtype::network::CasinoRoomListing{
      .rooms = {roomOf(HALL_FULL_ROOM, HALL_ROOM_SEATS),
                roomOf(HALL_FREE_ROOM, 0)}};
  reopenWith(MotionPreference::Full);
}

void CasinoHallScreenFixture::reopenWith(MotionPreference motion) {
  screen_ = std::make_unique<CasinoHallScreen>(
      CasinoHallScreenParts{.navigator = navigator_,
                            .sounds = sounds_,
                            .audio = audio_,
                            .casino = casino_,
                            .playfieldPointAt = samePlayfieldPoint,
                            .picture = HallPicture{},
                            .motion = motion},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void CasinoHallScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CasinoHallScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->update(ONE_FRAME);
}

void CasinoHallScreenFixture::release(sf::Keyboard::Key key) {
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CasinoHallScreenFixture::click(sf::Vector2i windowPixel) {
  screen_->handleEvent(leftPress(windowPixel));
  screen_->handleEvent(leftRelease(windowPixel));
  screen_->update(ONE_FRAME);
}

void CasinoHallScreenFixture::wait(rtype::engine::Duration elapsed) {
  screen_->update(elapsed);
}

void CasinoHallScreenFixture::fillEveryRoom() {
  casino_.listedRooms = rtype::network::CasinoRoomListing{
      .rooms = {roomOf(HALL_FULL_ROOM, HALL_ROOM_SEATS)}};
  screen_->update(ONE_FRAME);
}

const RecordingNavigator& CasinoHallScreenFixture::navigator() const {
  return navigator_;
}

const RecordingCasinoChannel& CasinoHallScreenFixture::casino() const {
  return casino_;
}

const InterfaceCanvasRecord& CasinoHallScreenFixture::canvas() const {
  return canvas_;
}
