#pragma once

#include "ActionSet.hpp"
#include "BotSight.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/** @brief What decides what a bot holds. */
class IBotPilot {
 public:
  virtual ~IBotPilot() = default;

  /** @returns The actions held during a frame of @p elapsed, knowing
   * @p sight. */
  [[nodiscard]] virtual client::ActionSet advance(engine::Duration elapsed,
                                                  const BotSight& sight) = 0;
};

}  // namespace rtype::tools
