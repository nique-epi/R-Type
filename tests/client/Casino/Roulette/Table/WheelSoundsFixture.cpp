#include "WheelSoundsFixture.hpp"
#include <chrono>
#include <string>
#include <vector>
#include "CasinoRoomState.hpp"
#include "MotionPreference.hpp"
#include "RouletteReading.hpp"
#include "RouletteScreenFixtures.hpp"

using rtype::client::MotionPreference;
using rtype::client::RouletteReading;
using rtype::client::testing::spinningRoom;

void WheelSoundsFixture::see(const rtype::network::CasinoRoomState& room) {
  RouletteReading reading;
  reading.room = room;
  wheel_.follow(reading);
}

void WheelSoundsFixture::seeWithLessMotion(
    const rtype::network::CasinoRoomState& room) {
  RouletteReading reading;
  reading.room = room;
  reading.motion = MotionPreference::Reduced;
  wheel_.follow(reading);
}

rtype::network::CasinoRoomState WheelSoundsFixture::spinningWith(
    std::chrono::seconds left) {
  rtype::network::CasinoRoomState room = spinningRoom();
  room.serverTime = room.resultAt - left;
  return room;
}

const std::vector<std::string>& WheelSoundsFixture::played() const {
  return sounds_.played();
}
