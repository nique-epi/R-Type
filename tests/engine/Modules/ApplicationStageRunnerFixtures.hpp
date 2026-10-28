#pragma once

#include <memory>
#include <vector>
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "PacingModule.hpp"

/**
 * @returns @p pacing, then a StageModule SECOND_MODULE adding its work to
 * SIMULATION_STAGE and writing its runs in @p journal.
 */
std::vector<std::unique_ptr<rtype::engine::IModule>>
pacingThenSecondInSimulation(std::unique_ptr<PacingModule> pacing,
                             JournalModule& journal);
