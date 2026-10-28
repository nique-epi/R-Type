#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <vector>
#include "IModule.hpp"
#include "JournalModule.hpp"
#include "NetworkModule.hpp"

/**
 * @returns A RecordingModule named JOURNAL_MODULE_NAME writing in @p journal,
 * @p network, then a TickCountingModule counting the passes of the loop in
 * @p passes.
 */
std::vector<std::unique_ptr<rtype::engine::IModule>> journalNetworkThenCounter(
    JournalModule& journal,
    std::unique_ptr<rtype::server::NetworkModule> network,
    std::atomic<std::size_t>& passes);

/** @returns A network module listening on the loopback address, on a port the
 * system picks. */
std::unique_ptr<rtype::server::NetworkModule> networkOnLoopback();

/**
 * @brief Waits until @p passes reaches SERVER_RUN_TICKS, for THREAD_TIMEOUT at
 * most.
 * @returns Whether it reached it.
 */
bool waitForPasses(const std::atomic<std::size_t>& passes);
