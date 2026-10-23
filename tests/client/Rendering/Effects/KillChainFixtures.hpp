#pragma once

#include <vector>
#include "TimeConstants.hpp"

/** @returns Whether each of @p count kills, @p gap apart, is a big catch. */
std::vector<bool> bigCatchesOf(int count, rtype::engine::Duration gap);
