#pragma once

#include <string_view>

/** @brief What the journal says when the server starts. */
constexpr std::string_view SERVER_STARTING_LINE_TEXT = "server starting";

/** @brief What the journal says when the server stops. */
constexpr std::string_view SERVER_STOPPED_LINE_TEXT = "server stopped";
