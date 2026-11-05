#include "FakeServerTicks.hpp"
#include <optional>

void FakeServerTicks::set(std::optional<double> tick) { tick_ = tick; }

std::optional<double> FakeServerTicks::serverTickNow() const { return tick_; }
