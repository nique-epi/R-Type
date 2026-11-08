#include <gtest/gtest.h>
#include "BeamOverlap.hpp"
#include "BeamOverlapTestConstants.hpp"

using rtype::game::beamOverlaps;
using rtype::game::testing::BEAM_HEADING_RIGHT;
using rtype::game::testing::BEAM_TILTED_DOWN;
using rtype::game::testing::BOX_ACROSS_THE_MIDDLE;
using rtype::game::testing::BOX_BESIDE_THE_TILTED_BEAM;
using rtype::game::testing::BOX_OFF_THE_END;

/**
 * Given a beam heading right and a small box across its middle
 * When their overlap is checked
 * Then they overlap
 */
TEST(BeamOverlap, OverlapsABoxAcrossItsMiddle) {
  EXPECT_TRUE(beamOverlaps(BEAM_HEADING_RIGHT, BOX_ACROSS_THE_MIDDLE));
}

/**
 * Given a beam heading right and a box on its line past its end
 * When their overlap is checked
 * Then they are apart
 */
TEST(BeamOverlap, IsApartFromABoxOffItsEnd) {
  EXPECT_FALSE(beamOverlaps(BEAM_HEADING_RIGHT, BOX_OFF_THE_END));
}

/**
 * Given a beam tilted 45 degrees and a box inside the rectangle that bounds
 * the beam, but beside its line
 * When their overlap is checked
 * Then they are apart: the axis across the beam separates them
 */
TEST(BeamOverlap, IsApartFromABoxBesideATiltedBeam) {
  EXPECT_FALSE(beamOverlaps(BEAM_TILTED_DOWN, BOX_BESIDE_THE_TILTED_BEAM));
}
