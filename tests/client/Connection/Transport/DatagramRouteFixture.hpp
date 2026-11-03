#pragma once

#include <gtest/gtest.h>
#include "DatagramRoute.hpp"
#include "RecordingDatagramLink.hpp"
#include "RecordingDatagramPump.hpp"

namespace rtype::client::testing {

/** @brief A route over a recording socket and a pump that counts its calls. */
class DatagramRouteFixture : public ::testing::Test {
 protected:
  [[nodiscard]] DatagramRoute& route();

  [[nodiscard]] const RecordingDatagramPump& pump() const;

 private:
  RecordingDatagramLink socket_;
  RecordingDatagramPump pump_;
  DatagramRoute route_{socket_, pump_};
};

}  // namespace rtype::client::testing
