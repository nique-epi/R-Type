#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include "EffectLengths.hpp"
#include "GainAttribution.hpp"
#include "Position.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::GainAttribution;
using rtype::game::Position;

namespace {

constexpr int FIRST_PILOT = 0;
constexpr int SECOND_PILOT = 1;
constexpr int BEFORE_THE_FIRST_PILOT = -1;
constexpr int AFTER_THE_LAST_PILOT = 4;
constexpr std::uint32_t SCORE_BEFORE = 1000;
constexpr std::uint32_t DRIFTER_POINTS = 100;
constexpr Position WHERE_IT_DIED{.x = 800.0F, .y = 400.0F};
constexpr Position WHERE_THE_LAST_DIED{.x = 900.0F, .y = 300.0F};

/** @returns An attribution that already knows the score of both pilots. */
GainAttribution knowingBothPilots() {
  GainAttribution attribution;
  attribution.noteScore(FIRST_PILOT, SCORE_BEFORE);
  attribution.noteScore(SECOND_PILOT, SCORE_BEFORE);
  return attribution;
}

}  // namespace

/**
 * Given a Bydo killed and a pilot the attribution never saw
 * When the first score of that pilot is noted
 * Then no figure: a first score is no gain
 */
TEST(GainAttribution, AFirstScoreIsNoGain) {
  GainAttribution attribution;
  attribution.noteKill(WHERE_IT_DIED, false);

  attribution.noteScore(FIRST_PILOT, SCORE_BEFORE);

  EXPECT_FALSE(attribution.takeFigure().has_value());
}

/**
 * Given a pilot at 1000 points and a Bydo killed
 * When its score reaches 1100
 * Then a figure of 100 shows where the Bydo died, in that pilot's color
 */
TEST(GainAttribution, OnePilotGainingForOneKillGetsItsColor) {
  GainAttribution attribution = knowingBothPilots();
  attribution.noteKill(WHERE_IT_DIED, false);

  attribution.noteScore(FIRST_PILOT, SCORE_BEFORE + DRIFTER_POINTS);

  const std::optional<GainAttribution::Figure> figure =
      attribution.takeFigure();
  ASSERT_TRUE(figure.has_value());
  const GainAttribution::Figure shown =
      figure.value_or(GainAttribution::Figure{});
  EXPECT_EQ(shown.points, DRIFTER_POINTS);
  EXPECT_EQ(shown.pilotColor, FIRST_PILOT);
  EXPECT_EQ(shown.place.x, WHERE_IT_DIED.x);
  EXPECT_EQ(shown.place.y, WHERE_IT_DIED.y);
}

/**
 * Given two pilots and one Bydo killed
 * When both gain 100 points
 * Then one figure of 200, in no pilot's color
 */
TEST(GainAttribution, TwoPilotsGainingShareAFigureInNoColor) {
  GainAttribution attribution = knowingBothPilots();
  attribution.noteKill(WHERE_IT_DIED, false);

  attribution.noteScore(FIRST_PILOT, SCORE_BEFORE + DRIFTER_POINTS);
  attribution.noteScore(SECOND_PILOT, SCORE_BEFORE + DRIFTER_POINTS);

  const std::optional<GainAttribution::Figure> figure =
      attribution.takeFigure();
  ASSERT_TRUE(figure.has_value());
  const GainAttribution::Figure shown =
      figure.value_or(GainAttribution::Figure{});
  EXPECT_EQ(shown.points, 2 * DRIFTER_POINTS);
  EXPECT_FALSE(shown.pilotColor.has_value());
}

/**
 * Given one pilot and two Bydos killed
 * When it gains 200 points
 * Then one figure at the place of the last kill, in no color
 */
TEST(GainAttribution, TwoKillsMakeOneFigureAtTheLastPlace) {
  GainAttribution attribution = knowingBothPilots();
  attribution.noteKill(WHERE_IT_DIED, false);
  attribution.noteKill(WHERE_THE_LAST_DIED, false);

  attribution.noteScore(FIRST_PILOT, SCORE_BEFORE + (2 * DRIFTER_POINTS));

  const std::optional<GainAttribution::Figure> figure =
      attribution.takeFigure();
  ASSERT_TRUE(figure.has_value());
  const GainAttribution::Figure shown =
      figure.value_or(GainAttribution::Figure{});
  EXPECT_EQ(shown.place.x, WHERE_THE_LAST_DIED.x);
  EXPECT_FALSE(shown.pilotColor.has_value());
}

/**
 * Given a pilot that gained 100 points
 * When a Bydo dies one nanosecond less than 150 ms later
 * Then the figure shows: a gain waits for its kill
 */
TEST(GainAttribution, AGainWaitsForItsKill) {
  GainAttribution attribution = knowingBothPilots();
  attribution.noteScore(FIRST_PILOT, SCORE_BEFORE + DRIFTER_POINTS);
  attribution.advance(GAIN_WINDOW_LENGTH - ONE_TICK);

  attribution.noteKill(WHERE_IT_DIED, false);

  EXPECT_TRUE(attribution.takeFigure().has_value());
}

/**
 * Given a Bydo killed
 * When 150 ms pass before a score grows
 * Then no figure: the kill was forgotten, and no gain is invented
 */
TEST(GainAttribution, AKillWithoutGainWithinTheWindowIsForgotten) {
  GainAttribution attribution = knowingBothPilots();
  attribution.noteKill(WHERE_IT_DIED, false);
  attribution.advance(GAIN_WINDOW_LENGTH);

  attribution.noteScore(FIRST_PILOT, SCORE_BEFORE + DRIFTER_POINTS);

  EXPECT_FALSE(attribution.takeFigure().has_value());
}

/**
 * Given a big catch
 * When its pilot gains
 * Then the figure is written large
 */
TEST(GainAttribution, ABigCatchMakesALargeFigure) {
  GainAttribution attribution = knowingBothPilots();
  attribution.noteKill(WHERE_IT_DIED, true);

  attribution.noteScore(FIRST_PILOT, SCORE_BEFORE + DRIFTER_POINTS);

  const std::optional<GainAttribution::Figure> figure =
      attribution.takeFigure();
  ASSERT_TRUE(figure.has_value());
  const GainAttribution::Figure shown =
      figure.value_or(GainAttribution::Figure{});
  EXPECT_TRUE(shown.big);
}

/**
 * Given a Bydo killed
 * When the colors just outside the pilots note two scores each, growing
 * Then no figure
 */
TEST(GainAttribution, IgnoresAColorOutsideThePilots) {
  GainAttribution attribution;
  attribution.noteKill(WHERE_IT_DIED, false);

  for (const int color : {BEFORE_THE_FIRST_PILOT, AFTER_THE_LAST_PILOT}) {
    attribution.noteScore(color, SCORE_BEFORE);
    attribution.noteScore(color, SCORE_BEFORE + DRIFTER_POINTS);
  }

  EXPECT_FALSE(attribution.takeFigure().has_value());
}

/**
 * Given a figure just taken
 * When it is asked for again
 * Then nothing: a gain shows once
 */
TEST(GainAttribution, AFigureShowsOnce) {
  GainAttribution attribution = knowingBothPilots();
  attribution.noteKill(WHERE_IT_DIED, false);
  attribution.noteScore(FIRST_PILOT, SCORE_BEFORE + DRIFTER_POINTS);
  ASSERT_TRUE(attribution.takeFigure().has_value());

  EXPECT_FALSE(attribution.takeFigure().has_value());
}
