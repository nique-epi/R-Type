#include <windows.h>
#include "FineTimerResolution.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

FineTimerResolution::FineTimerResolution()
    : granted_(timeBeginPeriod(FINE_TIMER_RESOLUTION_MILLISECONDS) ==
               TIMERR_NOERROR) {}

FineTimerResolution::~FineTimerResolution() {
  if (granted_) {
    static_cast<void>(timeEndPeriod(FINE_TIMER_RESOLUTION_MILLISECONDS));
  }
}

bool FineTimerResolution::isGranted() const { return granted_; }

}  // namespace rtype::server
