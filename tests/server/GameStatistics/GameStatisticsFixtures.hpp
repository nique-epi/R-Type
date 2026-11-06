#pragma once

#include <string>
#include <vector>

/** @returns The lines that load the compiled game statistics plugin: the
 * folder the build put it in, then its `plugin` line. */
[[nodiscard]] std::vector<std::string> gameStatisticsPluginLines();
