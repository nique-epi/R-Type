#pragma once

#include <cstdint>
#include <string>
#include <vector>

/** @returns The lines that load the compiled game statistics plugin: the
 * folder the build put it in, then its `plugin` line. */
[[nodiscard]] std::vector<std::string> gameStatisticsPluginLines();

/** @returns The file a game on @p port writes for a `game-statistics-file`
 * of STATISTICS_FILE_NAME: statistics_<port>.csv, in the same folder. */
[[nodiscard]] std::string statisticsFileOfPort(std::uint16_t port);
