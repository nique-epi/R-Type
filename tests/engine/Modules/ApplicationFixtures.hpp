#pragma once

#include <memory>
#include <vector>
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "RecordingModuleSetup.hpp"

/**
 * @returns One RecordingModule per setup, in the same order, all writing in @p
 * journal, which must outlive them.
 */
std::vector<std::unique_ptr<rtype::engine::IModule>> recordingModules(
    const std::vector<RecordingModuleSetup>& setups, JournalModule& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, which depend
 * on nothing. */
std::vector<std::unique_ptr<rtype::engine::IModule>> threeModules(
    JournalModule& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, which depend
 * on nothing; THIRD_MODULE throws from start(). */
std::vector<std::unique_ptr<rtype::engine::IModule>> thirdFailingToStart(
    JournalModule& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, which depend
 * on nothing; FIRST_MODULE throws from start(). */
std::vector<std::unique_ptr<rtype::engine::IModule>> firstFailingToStart(
    JournalModule& journal);

/** @returns FIRST_MODULE, then SECOND_MODULE, which depends on
 * UNLISTED_MODULE. */
std::vector<std::unique_ptr<rtype::engine::IModule>> secondDependingOnUnlisted(
    JournalModule& journal);

/** @returns FIRST_MODULE, which depends on SECOND_MODULE, then
 * SECOND_MODULE. */
std::vector<std::unique_ptr<rtype::engine::IModule>> firstDependingOnSecond(
    JournalModule& journal);

/** @returns FIRST_MODULE, then SECOND_MODULE, which depends on FIRST_MODULE. */
std::vector<std::unique_ptr<rtype::engine::IModule>> secondDependingOnFirst(
    JournalModule& journal);
