#pragma once

#include <string>
#include <vector>
#include "StageWork.hpp"
#include "TimeConstants.hpp"

/** @returns A work that appends @p line to @p ran each time it runs; @p ran
 * must outlive it. */
rtype::engine::StageWork recordingWork(std::vector<std::string>& ran,
                                       std::string line);

/** @returns A work that appends the time it is given to @p elapsedTimes each
 * time it runs; @p elapsedTimes must outlive it. */
rtype::engine::StageWork elapsedRecordingWork(
    std::vector<rtype::engine::Duration>& elapsedTimes);
