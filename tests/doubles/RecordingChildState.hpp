#pragma once

#include <optional>
#include "ProcessExit.hpp"

/** @brief What a recorded child went through, shared by the child and the
 * launcher that made it, so a test reads it after the owner dropped the
 * child. */
struct RecordingChildState {
  std::optional<rtype::server::ProcessExit> exit;
  bool terminated{false};
};
