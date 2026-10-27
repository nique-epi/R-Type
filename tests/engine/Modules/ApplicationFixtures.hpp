#pragma once

#include <memory>
#include <vector>
#include "IModule.hpp"
#include "ModuleJournal.hpp"
#include "RecordingModuleSetup.hpp"

/**
 * @returns One RecordingModule per setup, in the same order, all writing in @p
 * journal, which must outlive them.
 */
std::vector<std::unique_ptr<rtype::engine::IModule>> recordingModules(
    const std::vector<RecordingModuleSetup>& setups, ModuleJournal& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, which require
 * nothing. */
std::vector<std::unique_ptr<rtype::engine::IModule>> threeModules(
    ModuleJournal& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, which require
 * nothing; THIRD_MODULE throws from start(). */
std::vector<std::unique_ptr<rtype::engine::IModule>> thirdFailingToStart(
    ModuleJournal& journal);

/** @returns FIRST_MODULE, SECOND_MODULE and THIRD_MODULE, which require
 * nothing; FIRST_MODULE throws from start(). */
std::vector<std::unique_ptr<rtype::engine::IModule>> firstFailingToStart(
    ModuleJournal& journal);

/** @returns FIRST_MODULE, then SECOND_MODULE, which requires UNLISTED_MODULE.
 */
std::vector<std::unique_ptr<rtype::engine::IModule>> secondRequiringUnlisted(
    ModuleJournal& journal);

/** @returns FIRST_MODULE, which requires SECOND_MODULE, then SECOND_MODULE. */
std::vector<std::unique_ptr<rtype::engine::IModule>> firstRequiringSecond(
    ModuleJournal& journal);

/** @returns FIRST_MODULE, then SECOND_MODULE, which requires FIRST_MODULE. */
std::vector<std::unique_ptr<rtype::engine::IModule>> secondRequiringFirst(
    ModuleJournal& journal);
