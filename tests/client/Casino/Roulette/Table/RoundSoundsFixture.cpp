#include "RoundSoundsFixture.hpp"
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

void RoundSoundsFixture::see(const rtype::network::CasinoRoomState& room) {
  RouletteReading reading;
  reading.room = room;
  wheel_.follow(reading);
}

void RoundSoundsFixture::seeWithLessMotion(
    const rtype::network::CasinoRoomState& room) {
  RouletteReading reading;
  reading.room = room;
  reading.motion = MotionPreference::Reduced;
  wheel_.follow(reading);
}

rtype::network::CasinoRoomState RoundSoundsFixture::spinningWith(
    std::chrono::seconds left) {
  rtype::network::CasinoRoomState room = spinningRoom();
  room.serverTime = room.resultAt - left;
  return room;
}

const std::vector<std::string>& RoundSoundsFixture::played() const {
  return sounds_.played();
}
