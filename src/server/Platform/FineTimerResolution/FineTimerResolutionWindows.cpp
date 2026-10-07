#include <windows.h>  // NOLINT(misc-include-cleaner)
#include "FineTimerResolution.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

FineTimerResolution::FineTimerResolution() {
  // NOLINTNEXTLINE(misc-include-cleaner)
  timeBeginPeriod(FINE_TIMER_RESOLUTION_MILLISECONDS);
}

FineTimerResolution::~FineTimerResolution() {
  // NOLINTNEXTLINE(misc-include-cleaner)
  timeEndPeriod(FINE_TIMER_RESOLUTION_MILLISECONDS);
}

}  // namespace rtype::server
