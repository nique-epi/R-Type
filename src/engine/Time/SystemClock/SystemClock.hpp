#pragma once

#include "IClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief Clock backed by std::chrono::steady_clock, which never goes back.
 */
class SystemClock final : public IClock {
 public:
  [[nodiscard]] Duration now() const override;
};

}  // namespace rtype::engine
