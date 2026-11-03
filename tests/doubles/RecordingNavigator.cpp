#include "RecordingNavigator.hpp"
#include <optional>
#include <ostream>
#include <vector>
#include "ScreenId.hpp"

void PrintTo(const NavigatorCall& call, std::ostream* output) {
  *output << "request " << static_cast<int>(call.request);
  if (call.screen.has_value()) {
    *output << " of screen " << call.screen->name();
  }
}

void RecordingNavigator::push(rtype::client::ScreenId screen) {
  calls_.push_back({.request = NavigatorRequest::Push, .screen = screen});
}

void RecordingNavigator::pop() {
  calls_.push_back({.request = NavigatorRequest::Pop, .screen = std::nullopt});
}

void RecordingNavigator::replaceAll(rtype::client::ScreenId screen) {
  calls_.push_back({.request = NavigatorRequest::ReplaceAll, .screen = screen});
}

void RecordingNavigator::closeAll() {
  calls_.push_back(
      {.request = NavigatorRequest::CloseAll, .screen = std::nullopt});
}

const std::vector<NavigatorCall>& RecordingNavigator::calls() const {
  return calls_;
}
