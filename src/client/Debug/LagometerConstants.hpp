#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "InterfaceColorConstants.hpp"
#include "TrafficRecordingConstants.hpp"

namespace rtype::client {

/** @brief The key that shows and hides the lagometer, on every screen. */
constexpr sf::Keyboard::Scancode LAGOMETER_KEY = sf::Keyboard::Scan::F3;

/** @brief The round trips the curve keeps: 30 s at the two Heartbeats a
 * second of the protocol. */
constexpr std::size_t LAGOMETER_POINT_COUNT = 60;

/** @brief How often the frames per second and the traffic are counted again:
 * each figure is the mean over that second. */
constexpr std::chrono::seconds LAGOMETER_FIGURES_PERIOD{1};

/** @brief How often the lagometer writes a line in the journal: as often as
 * the traffic, so the two lines of a period sit next to each other. */
constexpr std::chrono::seconds LAGOMETER_JOURNAL_PERIOD =
    network::TRAFFIC_LINE_PERIOD;

/** @brief Module name the lines of the lagometer go under in the journal. */
constexpr std::string_view LAGOMETER_LOGGER_NAME = "Lagometer";

/** @brief The lowest top of the scale of the curve; the top doubles until it
 * is above the highest round trip shown. */
constexpr std::chrono::milliseconds LOWEST_LAGOMETER_SCALE_TOP{50};

/** @brief Percent, and the multiplier a share is written in percent with. */
constexpr std::uint64_t PERCENT = 100;

/** @brief Bytes in a kilobyte, as the traffic lines write it. */
constexpr std::uint64_t BYTES_PER_KILOBYTE = 1000;

/**
 * @brief Where the panel lies on the interface picture at a scale of 1, in
 * pixels: its top-left corner, its size, the margin inside its border, and
 * the rows of its four lines of text. Each is multiplied by the scale.
 */
constexpr sf::Vector2i LAGOMETER_TOP_LEFT{4, 4};
constexpr sf::Vector2i LAGOMETER_SIZE{112, 82};
constexpr int LAGOMETER_MARGIN = 4;
constexpr int LAGOMETER_LINE_STEP = 10;
constexpr int LAGOMETER_TEXT_LINE_COUNT = 4;

/** @brief The curve: the rows of the panel it covers, below the text, and the
 * width the labels of the scale take on its right. */
constexpr int LAGOMETER_CURVE_TOP = 46;
constexpr int LAGOMETER_CURVE_HEIGHT = 32;
constexpr int LAGOMETER_SCALE_LABEL_WIDTH = 24;

/** @brief Full opacity of a color, from which the fill of the panel takes the
 * share the player set. */
constexpr std::uint8_t LAGOMETER_OPAQUE_ALPHA = 255;

/** @brief Red of a lost datagram on the curve. */
constexpr sf::Color LOSS_RED(226, 75, 74);

/** @brief The colors of the panel: its fill, its border, its text, the curve
 * and the lines of its scale. */
constexpr sf::Color LAGOMETER_FILL = INK;
constexpr sf::Color LAGOMETER_BORDER = EDGE_BLUE;
constexpr sf::Color LAGOMETER_TEXT = HUD_CYAN;
constexpr sf::Color LAGOMETER_CURVE = PLAYER_GREEN;
constexpr sf::Color LAGOMETER_SCALE_LINE = PLAYER_DARK_BLUE;

/** @brief What the lines of the panel say before their figure is known. */
constexpr std::string_view NO_FIGURE_TEXT = "--";

/** @brief The words of the four lines of the panel, around their figures:
 * PING 42 MS JIT 3, LOSS 2% FPS 60, IN 12 OUT 4 KB/S, BUF 120 MS LATE 9. */
constexpr std::string_view PING_WORD = "PING ";
constexpr std::string_view JITTER_WORD = " JIT ";
constexpr std::string_view MILLISECONDS_WORD = " MS";
constexpr std::string_view LOSS_WORD = "LOSS ";
constexpr std::string_view PERCENT_WORD = "%";
constexpr std::string_view FRAMES_WORD = " FPS ";
constexpr std::string_view RECEIVED_WORD = "IN ";
constexpr std::string_view SENT_WORD = " OUT ";
constexpr std::string_view KILOBYTES_WORD = " KB/S";
constexpr std::string_view BUFFER_WORD = "BUF ";
constexpr std::string_view LATE_WORD = " LATE ";

/** @brief The words of the line of the journal, around its figures:
 * lagometer over 10 s: ping 85 ms, jitter 33 ms; missed 2 of 300 datagrams;
 * 600 frames (60/s), 54 drawn past the newest state (5/s); display 98 to 140
 * ms behind the server. */
constexpr std::string_view JOURNAL_PERIOD_WORD = "lagometer over ";
constexpr std::string_view JOURNAL_PING_WORD = " s: ping ";
constexpr std::string_view JOURNAL_JITTER_WORD = " ms, jitter ";
constexpr std::string_view JOURNAL_MISSED_WORD = " ms; missed ";
constexpr std::string_view JOURNAL_OF_WORD = " of ";
constexpr std::string_view JOURNAL_DATAGRAMS_WORD = " datagrams; ";
constexpr std::string_view JOURNAL_FRAMES_WORD = " frames (";
constexpr std::string_view JOURNAL_FRAMES_END_WORD = "/s), ";
constexpr std::string_view JOURNAL_PAST_THE_NEWEST_WORD =
    " drawn past the newest state (";
constexpr std::string_view JOURNAL_DISPLAY_WORD = "/s); display ";
constexpr std::string_view JOURNAL_TO_WORD = " to ";
constexpr std::string_view JOURNAL_BEHIND_WORD = " ms behind the server";

}  // namespace rtype::client
