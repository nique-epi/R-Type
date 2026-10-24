#include "GainAttributionFixtures.hpp"
#include "EffectTestConstants.hpp"
#include "GainAttribution.hpp"
#include "PilotTestConstants.hpp"

using rtype::client::GainAttribution;

GainAttribution knowingBothPilots() {
  GainAttribution attribution;
  attribution.noteScore(FIRST_PILOT_COLOR, SCORE_BEFORE);
  attribution.noteScore(SECOND_PILOT_COLOR, SCORE_BEFORE);
  return attribution;
}
