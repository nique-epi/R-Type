#include "RecordingDeviceLinkFlow.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include "DeviceLinkReading.hpp"

std::uint64_t RecordingDeviceLinkFlow::askLink() { return ++linksAsked_; }

void RecordingDeviceLinkFlow::askCancel() { ++cancels_; }

void RecordingDeviceLinkFlow::askSignOut(const std::string& token) {
  signedOutToken_ = token;
}

std::optional<rtype::client::DeviceLinkReading>
RecordingDeviceLinkFlow::reading() const {
  return reading_;
}

void RecordingDeviceLinkFlow::publish(
    const rtype::client::DeviceLinkReading& reading) {
  reading_ = reading;
}

std::uint64_t RecordingDeviceLinkFlow::linksAsked() const {
  return linksAsked_;
}

int RecordingDeviceLinkFlow::cancels() const { return cancels_; }

const std::string& RecordingDeviceLinkFlow::signedOutToken() const {
  return signedOutToken_;
}
