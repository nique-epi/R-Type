#include "WheelMotion.hpp"
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include "CasinoSpriteConstants.hpp"
#include "MotionPreference.hpp"
#include "RoulettePhase.hpp"
#include "TimeConstants.hpp"
#include "WheelConstants.hpp"
#include "WheelMoment.hpp"
#include "WheelPicture.hpp"
#include "WheelSlowdownStep.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The frames of @p frameTime that @p time begins, a frame begun
 * counted whole. */
int framesBegunIn(engine::Duration time, engine::Duration frameTime) {
  return static_cast<int>((time + frameTime - engine::Duration(1)) / frameTime);
}

/** @returns The place of @p number on the wheel, clockwise from the zero. */
int pocketOf(std::uint8_t number) {
  for (std::size_t pocket = 0; pocket < WHEEL_ORDER.size(); ++pocket) {
    if (WHEEL_ORDER.at(pocket) == number) {
      return static_cast<int>(pocket);
    }
  }
  return 0;
}

/** @returns The top-left corner of the ball sprite whose middle lies
 * @p radius from the middle of the wheel, at @p angle radians clockwise from
 * the right. */
sf::Vector2i ballAt(double angle, double radius) {
  const double middle = static_cast<double>(WHEEL_FRAME_SIZE.x) /
                        static_cast<double>(SIDES_PER_AXIS);
  return {static_cast<int>(std::lround(middle + (std::cos(angle) * radius))) -
              BALL_CENTER,
          static_cast<int>(std::lround(middle + (std::sin(angle) * radius))) -
              BALL_CENTER};
}

/** @returns Where the ball lies in the pocket of @p number while the rotor
 * shows @p rotorFrame. */
sf::Vector2i ballInPocket(std::uint8_t number, int rotorFrame) {
  const double pocketAngle =
      FULL_TURN_RADIANS / static_cast<double>(WHEEL_ORDER.size());
  const double pockets =
      pocketOf(number) + (static_cast<double>(rotorFrame) /
                          static_cast<double>(ROTOR_FRAMES_PER_POCKET));
  return ballAt(TOP_OF_THE_WHEEL_RADIANS + (pockets * pocketAngle),
                BALL_POCKET_RADIUS);
}

/** @returns Where the ball runs on the track with @p timeLeft left of the
 * spin, against the rotor, so it passes the top as the spin ends. */
sf::Vector2i ballOnTrack(engine::Duration timeLeft) {
  const double seconds = std::chrono::duration<double>(timeLeft).count();
  return ballAt(TOP_OF_THE_WHEEL_RADIANS +
                    (FULL_TURN_RADIANS * BALL_TURNS_PER_SECOND * seconds),
                BALL_TRACK_RADIUS);
}

/** @returns What the wheel shows while it spins, @p moment.timeLeft before
 * it stops on the number drawn. */
WheelPicture spinningPicture(const WheelMoment& moment) {
  const int framesLeft = rotorFramesLeft(moment.timeLeft);
  const int frame = (((stoppingFrameOf(moment.winningNumber) - framesLeft) %
                      WHEEL_ROTOR_TURN_FRAMES) +
                     WHEEL_ROTOR_TURN_FRAMES) %
                    WHEEL_ROTOR_TURN_FRAMES;
  return {.rotorFrame = frame,
          .ball = framesLeft <= BALL_DROP_FRAMES
                      ? ballInPocket(moment.winningNumber, frame)
                      : ballOnTrack(moment.timeLeft)};
}

}  // namespace

int stoppingFrameOf(std::uint8_t number) {
  return (WHEEL_ROTOR_TURN_FRAMES -
          (pocketOf(number) * ROTOR_FRAMES_PER_POCKET)) %
         WHEEL_ROTOR_TURN_FRAMES;
}

int rotorFramesLeft(engine::Duration timeLeft) {
  int frames = 0;
  engine::Duration left = timeLeft;
  for (const WheelSlowdownStep& step : ROTOR_SLOWDOWN) {
    if (left <= step.frameTime * step.frames) {
      return frames + framesBegunIn(left, step.frameTime);
    }
    left -= step.frameTime * step.frames;
    frames += step.frames;
  }
  return frames + framesBegunIn(left, ROTOR_FASTEST_FRAME_TIME);
}

WheelPicture wheelPictureAt(const WheelMoment& moment,
                            MotionPreference motion) {
  switch (moment.phase) {
    case network::RoulettePhase::Betting:
      return {};
    case network::RoulettePhase::Spinning:
      if (motion == MotionPreference::Reduced) {
        return {};
      }
      return spinningPicture(moment);
    case network::RoulettePhase::Result:
      break;
  }
  const int frame = stoppingFrameOf(moment.winningNumber);
  return {.rotorFrame = frame,
          .ball = ballInPocket(moment.winningNumber, frame)};
}

}  // namespace rtype::client
