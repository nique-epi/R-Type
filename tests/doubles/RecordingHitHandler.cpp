#include "RecordingHitHandler.hpp"
#include <string_view>
#include <vector>
#include "AppliedHit.hpp"

namespace rtype::game::testing {

RecordingHitHandler::RecordingHitHandler(std::string_view name) : name_(name) {}

std::string_view RecordingHitHandler::name() const { return name_; }

void RecordingHitHandler::onHit(const AppliedHit& hit) { told_.push_back(hit); }

const std::vector<AppliedHit>& RecordingHitHandler::told() const {
  return told_;
}

}  // namespace rtype::game::testing
