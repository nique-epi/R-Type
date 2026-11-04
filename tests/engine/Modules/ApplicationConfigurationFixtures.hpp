#pragma once

#include <string>
#include <vector>
#include "ConfigurationStep.hpp"
#include "JournalModule.hpp"
#include "LoopStages.hpp"

/**
 * @returns A configuration step that writes CONFIGURE_STEP in @p journal, then
 * runs each of @p lines as a command and keeps in @p refusals the message of
 * each line it refused, those of its last run only, as configurationStep()
 * keeps the report of its last run. @p journal and @p refusals must outlive
 * the step.
 */
rtype::engine::ConfigurationStep runningLines(
    std::vector<std::string> lines, JournalModule& journal,
    std::vector<std::string>& refusals);

/** @returns A loop that has no stage. */
rtype::engine::LoopStages withoutStages();
