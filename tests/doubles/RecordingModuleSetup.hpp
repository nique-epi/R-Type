#pragma once

#include <string>
#include <vector>

/** @brief What a RecordingModule answers and how it behaves. */
struct RecordingModuleSetup {
  std::string name;
  std::vector<std::string> dependencies;
  /** @brief Whether start() throws a StartRefusedException after writing its
   * call down. */
  bool failsToStart{false};
};
