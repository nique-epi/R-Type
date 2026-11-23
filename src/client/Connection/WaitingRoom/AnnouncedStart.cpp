#include "AnnouncedStart.hpp"
#include <optional>
#include <utility>
#include "GameStarted.hpp"

namespace rtype::client {

void AnnouncedStart::hold(network::GameStarted started) {
  started_ = std::move(started);
}

void AnnouncedStart::forget() { started_.reset(); }

const std::optional<network::GameStarted>& AnnouncedStart::started() const {
  return started_;
}

}  // namespace rtype::client
