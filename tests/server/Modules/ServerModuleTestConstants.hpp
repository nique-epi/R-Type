#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief A command of the administration console, and the line that turns it
 * off, as server.cfg holds them. */
constexpr std::string_view GAMES_LINE = "games";
constexpr std::string_view CONSOLE_OFF_LINE = "module \"admin console\" off";

/** @brief The names of the probe modules the tests list after the modules of
 * the server. */
constexpr const char* PROBE_MODULE = "probe";
constexpr const char* PORT_PROBE_MODULE = "port probe";

/** @brief The lines that load the game-statistics plugin, as server.cfg
 * writes them; a test that only reads lines needs no compiled plugin. */
constexpr std::string_view PLUGIN_LINE = "plugin game-statistics";
constexpr std::string_view PLUGIN_FOLDER_LINE = "plugin-folder plugins";

/** @brief The database of a server a test starts, in a folder of the test. */
constexpr std::string_view TEST_DATABASE_FILE = "results.db";

/** @brief The file of commands a test gives a game process after
 * `--configuration`, in the folder it starts in. */
constexpr std::string_view GAME_CONFIGURATION_FILE = "games.cfg";

/** @brief The token the game process a test starts shares with its
 * supervisor, as its command line writes it. */
constexpr std::string_view GAME_PROCESS_LINK_TOKEN = "4660";

/** @brief The text the failing stage work of the tests throws. */
constexpr const char* STAGE_FAILURE_TEXT = "a stage failed";

/** @brief The session token of the player the tests admit. */
constexpr std::uint64_t ADMITTED_TOKEN = 0xAB12;

/** @brief The seed of the games the tests start, and the name and the first
 * session token of the players they connect. */
constexpr std::uint32_t SOME_SEED = 7;
constexpr const char* PLAYER_NICKNAME = "ace";
constexpr std::uint64_t PLAYER_TOKEN_BASE = 0x1000;

/** @brief A waves file that is not there. */
constexpr const char* MISSING_WAVES_FILE = "no/such/folder/waves.toml";

/** @brief A content folder that is not there. */
constexpr const char* MISSING_CONTENT_FOLDER = "no/such/folder";

/** @brief The example resource the repository ships beside its assets, from
 * the assets folder, and the waves file it brings in place of theirs. */
constexpr const char* EXAMPLE_RESOURCE_FROM_THE_ASSETS =
    "../resources/crimson-drifter";
constexpr const char* WAVES_OF_THE_EXAMPLE_RESOURCE = "waves/waves.toml";

/** @brief Lines that set the rules of the game: the difficulty "hard", five
 * lives, friendly fire on; and a difficulty the shipped waves file has no
 * table for. */
constexpr std::string_view SET_HARD_GAME_LINE = "set difficulty hard";
constexpr std::string_view SET_FIVE_LIVES_GAME_LINE = "set lives 5";
constexpr std::string_view SET_FRIENDLY_FIRE_GAME_LINE = "set friendly-fire 1";
constexpr std::string_view SET_UNKNOWN_DIFFICULTY_LINE =
    "set difficulty nightmare";

/** @brief The line of the journal that says the game plays those rules, and is
 * not vanilla. */
constexpr const char* RULED_GAMES_LINE =
    "games play hard, 5 lives, friendly fire on, not vanilla";

/** @brief The line of the journal that says the game plays the default rules
 * and is vanilla. */
constexpr const char* VANILLA_GAMES_LINE =
    "games play normal, 3 lives, friendly fire off, vanilla";

/** @brief How long the client of a test waits for the answer of the server to a
 * request, and for the farewell that follows the stop. */
constexpr std::chrono::milliseconds WAIT_FOR_ANSWER{5000};
constexpr std::chrono::milliseconds GOODBYE_WAIT{400};

/** @brief A count of datagrams the client never reaches, so it waits for the
 * whole of GOODBYE_WAIT. */
constexpr std::size_t UNREACHABLE_COUNT = 100000;

/** @brief The least time between two copies of the farewell as the client sees
 * it: FAREWELL_COPY_INTERVAL (50 ms) less what a loaded machine takes away
 * from one of two neighbours. */
constexpr std::chrono::milliseconds MINIMUM_COPY_SPACING{20};

/** @brief How long a test waits for something a thread of the server does. */
constexpr std::chrono::seconds WAIT_FOR_THREADS{5};

/** @brief How many passes of the loop a test waits for. */
constexpr int PASSES_TO_WAIT_FOR = 2;

/**
 * @brief A line that gives the state rate setting a rate above the highest one.
 */
constexpr std::string_view STATE_RATE_ABOVE_THE_HIGHEST_LINE =
    "set state-rate 1000";

/**
 * @brief A line that clears the seed setting.
 */
constexpr std::string_view CLEAR_THE_SEED_LINE = "set seed none";

/**
 * @brief A line that gives the state encoding a value that names no encoding,
 * and one that asks for every state in full.
 */
constexpr std::string_view UNKNOWN_STATE_ENCODING_LINE =
    "set state-encoding fast";
constexpr std::string_view FULL_STATE_ENCODING_LINE = "set state-encoding full";

/** @brief The only byte of the datagram a test queues for its client. */
constexpr std::byte QUEUED_BYTE{0x2A};
