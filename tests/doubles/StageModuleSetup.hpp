#pragma once

#include <string>

/** @brief The name of a StageModule, the stage it adds its work to and how it
 * behaves on start(). */
struct StageModuleSetup {
  std::string name;
  std::string stage;
  /** @brief Whether start() throws a StartRefusedException. */
  bool failsToStart{false};
};
