#include "IntroPainting.hpp"
#include "IInterfaceCanvas.hpp"
#include "IntroBoostPainting.hpp"
#include "IntroFirePainting.hpp"
#include "IntroFleetPainting.hpp"
#include "IntroFrame.hpp"

namespace rtype::client {

void paintIntroCast(IInterfaceCanvas& canvas, const IntroFrame& frame) {
  paintMobs(canvas, frame);
  paintEnemyMissiles(canvas, frame);
  paintShips(canvas, frame);
  paintPlayerMissiles(canvas, frame);
  paintBursts(canvas, frame);
  paintRings(canvas, frame);
  paintCharges(canvas, frame);
  paintBeams(canvas, frame);
  paintSpeedLines(canvas, frame);
}

}  // namespace rtype::client
