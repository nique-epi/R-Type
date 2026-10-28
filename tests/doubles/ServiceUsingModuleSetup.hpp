#pragma once

#include <string>

/** @brief The name of a ServiceUsingModule and how it asks for the
 * TestService. */
struct ServiceUsingModuleSetup {
  std::string name;
  /** @brief Whether install() requires the TestService, or only looks for
   * it. */
  bool requiresService{false};
};
