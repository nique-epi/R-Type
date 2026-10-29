#pragma once

#include <stdexcept>

/** @brief What a RecordingModule set to fail throws from start(); it stands for
 * any error a real module may throw. */
class StartRefusedException : public std::runtime_error {
 public:
  StartRefusedException();
};
