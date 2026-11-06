#pragma once

#include "TrafficTotals.hpp"

/** @brief The totals of a socket, the same totals once more datagrams crossed
 * it, and what crossed in between. */
constexpr rtype::network::TrafficTotals EARLIER_TOTALS{
    .sent = {.count = 10, .bytes = 300},
    .received = {.count = 20, .bytes = 4000}};
constexpr rtype::network::TrafficTotals LATER_TOTALS{
    .sent = {.count = 11, .bytes = 330},
    .received = {.count = 22, .bytes = 4800}};
constexpr rtype::network::TrafficTotals TRAFFIC_IN_BETWEEN{
    .sent = {.count = 1, .bytes = 30}, .received = {.count = 2, .bytes = 800}};
