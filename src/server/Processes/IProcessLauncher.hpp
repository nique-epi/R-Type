#pragma once

#include <memory>
#include "IChildProcess.hpp"
#include "ProcessCommand.hpp"

namespace rtype::server {

/** @brief Starts programs as child processes of the one that holds it. */
class IProcessLauncher {
 public:
  virtual ~IProcessLauncher() = default;

  /**
   * @brief Starts @p command.
   * @returns The child, which dies with its parent however the parent ends
   * (see ChildBinding).
   * @throws ProcessLaunchException when the system cannot start it.
   * @throws ChildBindingException when it started but could not be tied to its
   * parent; it is killed first.
   */
  [[nodiscard]] virtual std::unique_ptr<IChildProcess> launch(
      ProcessCommand command) = 0;
};

}  // namespace rtype::server
