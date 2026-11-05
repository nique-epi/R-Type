#pragma once

#include <optional>
#include "IServerTicks.hpp"

/**
 * @brief Server ticks double: the tick the test set, or none until it sets
 * one, as a client that does not know the clock of the server yet.
 */
class FakeServerTicks final : public rtype::client::IServerTicks {
 public:
  /** @brief Makes the server be at @p tick from now on; none forgets it. */
  void set(std::optional<double> tick);

  [[nodiscard]] std::optional<double> serverTickNow() const override;

 private:
  std::optional<double> tick_;
};
