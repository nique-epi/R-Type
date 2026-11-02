#pragma once

namespace rtype::client {

/** @brief The values an integer setting accepts, both ends included. */
struct SettingRange {
  int minimum = 0;
  int maximum = 0;
};

}  // namespace rtype::client
