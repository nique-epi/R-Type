#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "NetworkConditions.hpp"

/** @brief The seed of most tests, and another to compare with. */
constexpr std::uint32_t SOME_SEED = 7;
constexpr std::uint32_t OTHER_SEED = 8;

/** @brief When the tests hand their first datagram over: any time of the
 * steady clock. */
constexpr std::chrono::steady_clock::time_point HAND_OVER_START{
    std::chrono::hours{1}};

/** @brief The time of a second hand over while the first datagram is in
 * flight: less than half the lag of LAGGING_NETWORK. */
constexpr std::chrono::milliseconds SECOND_HAND_OVER{20};

/**
 * @brief A jitter of 60 ms, the half a datagram may wait more, and two waits
 * that 1000 draws all but surely go below and above: the chance that none of
 * them waits less than 5 ms of the 30 ms is (25/30)^1000.
 */
constexpr rtype::network::NetworkConditions JITTER_OF_60_MS{
    .jitter = std::chrono::milliseconds{60}};
constexpr std::chrono::milliseconds HALF_OF_60_MS{30};
constexpr std::chrono::milliseconds NEAR_NO_WAIT{5};
constexpr std::chrono::milliseconds NEAR_HALF_OF_60_MS{25};

/** @brief The shares of loss, duplicates and reordering the tests set. */
constexpr rtype::network::NetworkConditions FULL_LOSS{.lossPercent = 100};
constexpr rtype::network::NetworkConditions TENTH_LOSS{.lossPercent = 10};
constexpr rtype::network::NetworkConditions FULL_DUPLICATES{.duplicatePercent =
                                                                100};
constexpr rtype::network::NetworkConditions TENTH_DUPLICATES{.duplicatePercent =
                                                                 10};
constexpr rtype::network::NetworkConditions HALF_LOSS_FULL_DUPLICATES{
    .lossPercent = 50, .duplicatePercent = 100};
constexpr rtype::network::NetworkConditions FULL_REORDER{.reorderPercent = 100};
constexpr rtype::network::NetworkConditions HALF_REORDER{.reorderPercent = 50};

/** @brief Every defect at once, to compare the draws of two seeds. */
constexpr rtype::network::NetworkConditions EVERY_DEFECT{
    .lag = std::chrono::milliseconds{100},
    .jitter = std::chrono::milliseconds{60},
    .lossPercent = 10,
    .duplicatePercent = 10,
    .reorderPercent = 10};

/** @brief How much longer the plan holds a reordered datagram. */
constexpr std::chrono::milliseconds HOLD_OF_A_REORDERED_DATAGRAM{75};

/** @brief How many datagrams the tests hand over, and how far apart: closer
 * than the jitter, closer than the reorder hold, or farther than both. */
constexpr std::size_t THOUSAND_DATAGRAMS = 1000;
constexpr std::size_t TEN_THOUSAND_DATAGRAMS = 10000;
constexpr std::chrono::milliseconds ONE_MILLISECOND_APART{1};
constexpr std::chrono::milliseconds TEN_MILLISECONDS_APART{10};
constexpr std::chrono::milliseconds HUNDRED_MILLISECONDS_APART{100};

/**
 * @brief The arrivals of 10 000 datagrams at a tenth of loss or of duplicates,
 * and the gap the tests allow: five standard deviations of 10 000 draws at
 * 10 %, sqrt(10000 * 0.1 * 0.9) = 30.
 */
constexpr double ARRIVALS_AT_A_TENTH_LOSS = 9000.0;
constexpr double ARRIVALS_AT_A_TENTH_DUPLICATES = 11000.0;
constexpr double DRAW_TOLERANCE = 150.0;

/** @brief How many times a copied datagram arrives. */
constexpr std::size_t ARRIVALS_OF_A_COPIED_DATAGRAM = 2;
