#pragma once

#include <string_view>

/** @brief The words of BAD_OF_THE_TABLE, as the journal and the usage write
 * them. */
constexpr std::string_view BAD_NETWORK_IN_WORDS =
    "ping +300 ms, up to 50 ms more, 10 % lost, 2 % duplicated, 5 % reordered";
