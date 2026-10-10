#include <windows.h>  // NOLINT(misc-include-cleaner)
#include "FineTimerResolution.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

// NOLINTBEGIN(misc-include-cleaner)
FineTimerResolution::FineTimerResolution()
    : granted_(timeBeginPeriod(FINE_TIMER_RESOLUTION_MILLISECONDS) ==
               TIMERR_NOERROR) {}

FineTimerResolution::~FineTimerResolution() {
  if (granted_) {
    static_cast<void>(timeEndPeriod(FINE_TIMER_RESOLUTION_MILLISECONDS));
  }
}
// NOLINTEND(misc-include-cleaner)

bool FineTimerResolution::isGranted() const { return granted_; }

}  // namespace rtype::server
