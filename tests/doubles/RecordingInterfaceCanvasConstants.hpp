#pragma once

/** @brief Advance a recording canvas gives each character of small print:
 * Silkscreen is proportional (3 to 7 pixels), so the double takes 6, the
 * width of most of its capitals; the regular font keeps its real
 * REGULAR_FONT_ADVANCE. */
constexpr int RECORDED_SMALL_ADVANCE = 6;
