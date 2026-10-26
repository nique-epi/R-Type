#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include "CinemaView.hpp"
#include "IntroFreeze.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns The time of the scene at @p real seconds of the music: @p real
 * less the part of each freeze already gone. */
[[nodiscard]] float sceneTimeAt(float real,
                                std::span<const IntroFreeze> freezes);

/** @returns How many times their own speed the stars move at that moment of
 * the scene; see INTRO_COLD_OPEN_SPEED. */
[[nodiscard]] float skyFactorAt(float scene);

/** @returns The part of the world shown at @p real seconds of the music: the
 * Bydo in the dark until the first beat, then each pilot until
 * INTRO_SWARM_CUT, both zoomed INTRO_CLOSEUP_ZOOM times, then the whole
 * world. */
[[nodiscard]] CinemaView cinemaViewAt(float real);

/** @returns How far left the debris drifted at that moment of the scene: 0
 * until the boost, then INTRO_DEBRIS_SPEED times the way the sky went since
 * the boost. */
[[nodiscard]] float debrisDriftAt(float scene);

/** @returns The moment the close-up of @p pilot starts: the beat after the
 * number of the pilot, the first beat for the first pilot. */
[[nodiscard]] float closeupStartOf(std::size_t pilot);

/** @returns The time of the intro a skip at @p now jumps to: INTRO_SLAM less
 * the time until the next beat of the music at @p music seconds, so the logo
 * slams on that beat, a beat later at most; INTRO_SLAM without music; never
 * before @p now. */
[[nodiscard]] float skipTargetAt(float now, std::optional<float> music);

/** @returns Whether @p time falls in an odd step, steps changing
 * @p stepsPerSecond times a second: what makes a flame, a charge or a
 * tremble flicker. */
[[nodiscard]] bool isOddStepAt(float time, float stepsPerSecond);

/** @returns The duration of @p seconds, and the seconds of a duration: the
 * intro counts in seconds, the frame loop in durations. */
[[nodiscard]] engine::Duration durationOf(float seconds);
[[nodiscard]] float secondsOf(engine::Duration duration);

}  // namespace rtype::client
