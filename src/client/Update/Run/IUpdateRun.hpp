#pragma once

#include "UpdateProgress.hpp"

namespace rtype::client {

/** @brief An update in progress, as the update screen drives it from the
 * frame loop. */
class IUpdateRun {
 public:
  virtual ~IUpdateRun() = default;

  /** @returns Where the update stands; cheap, read at each frame. */
  [[nodiscard]] virtual UpdateProgress progress() const = 0;

  /** @brief Starts the new version, once ReadyToRestart; the phase becomes
   * Failed when it could not start.
   * @returns Whether it started, the client then to close. */
  [[nodiscard]] virtual bool restart() = 0;
};

}  // namespace rtype::client
