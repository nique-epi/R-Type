#pragma once

/** @brief The name of a test plugin no file is called after. */
constexpr const char* GHOST_LIBRARY_NAME = "ghost";

/** @brief The name of the test plugin that exports an entry. */
constexpr const char* LIBRARY_WITH_ENTRY_NAME = "probe";

/** @brief A symbol no test plugin exports. */
constexpr const char* UNEXPORTED_SYMBOL_NAME = "noSuchSymbol";
