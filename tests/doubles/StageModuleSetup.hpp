#pragma once

#include <string>

/** @brief The name of a StageModule and the stage it adds its work to. */
struct StageModuleSetup {
  std::string name;
  std::string stage;
};
