#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

/** @brief The name the probe of the tests is exposed under. */
constexpr std::string_view PROBE_TABLE = "probe";

/** @brief A script that records a number through the probe, and the number. */
constexpr std::string_view RECORDS_THE_ANSWER = "probe.record(42)";
constexpr double THE_ANSWER = 42.0;

/** @brief A script that hands the probe a function doubling its argument, the
 * argument the test passes and what the function must return. */
constexpr std::string_view HANDS_OVER_A_DOUBLING =
    "probe.keep(function(value) return value * 2 end)";
constexpr double DOUBLED_ARGUMENT = 21.0;
constexpr double DOUBLED_RESULT = 42.0;

/** @brief A script that never ends. */
constexpr std::string_view LOOPS_FOREVER = "while true do end";

/** @brief A script that asks for 4 MiB of text, and a limit of 1 MiB. */
constexpr std::string_view HOLDS_FOUR_MEBIBYTES =
    "local text = string.rep('x', 4 * 1024 * 1024)";
constexpr std::size_t ONE_MEBIBYTE = std::size_t{1024} * 1024;

/** @brief A budget long enough that only the memory can stop a script. */
constexpr std::chrono::milliseconds GENEROUS_BUDGET{2000};

/** @brief A script that fails when anything of the system or of another
 * script's globals can be reached. */
constexpr std::string_view CHECKS_THE_SANDBOX =
    "assert(io == nil)\n"
    "assert(os.execute == nil)\n"
    "assert(require == nil)\n"
    "assert(getfenv == nil and setfenv == nil)\n"
    "assert(dofile == nil and loadfile == nil and load == nil)\n";

/** @brief Two scripts: one sets a global, the other expects not to see it. */
constexpr std::string_view SETS_A_GLOBAL = "shared = 1";
constexpr std::string_view EXPECTS_NO_SUCH_GLOBAL = "assert(shared == nil)";

/** @brief A script that changes a standard library. */
constexpr std::string_view CHANGES_THE_STRING_LIBRARY = "string.rep = nil";

/** @brief A script that does not compile on its second line, its name and
 * where its error must point. */
constexpr std::string_view DOES_NOT_COMPILE = "local value = 1\nend\n";
constexpr std::string_view BROKEN_SCRIPT_NAME = "broken.luau";
constexpr std::string_view BROKEN_SCRIPT_PLACE = "broken.luau:2";

/** @brief The name of a script that compiles. */
constexpr std::string_view SCRIPT_NAME = "test.luau";
