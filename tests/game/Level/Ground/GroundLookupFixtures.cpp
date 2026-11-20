#include "GroundLookupFixtures.hpp"
#include "BiomeGround.hpp"
#include "GroundLookupTestConstants.hpp"

rtype::game::BiomeGround groundOfFourColumns() {
  return rtype::game::BiomeGround{
      .floorRows = {FLOOR_ROWS_OF_FOUR_COLUMNS.begin(),
                    FLOOR_ROWS_OF_FOUR_COLUMNS.end()},
      .ceilingRows = {CEILING_ROWS_OF_FOUR_COLUMNS.begin(),
                      CEILING_ROWS_OF_FOUR_COLUMNS.end()}};
}
