#include "FineTimerResolution.hpp"

namespace rtype::server {

FineTimerResolution::FineTimerResolution() = default;

FineTimerResolution::~FineTimerResolution() = default;

bool FineTimerResolution::isGranted() const { return granted_; }

}  // namespace rtype::server
