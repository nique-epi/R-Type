#include "PaddedScore.hpp"
#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include "IInterfaceCanvas.hpp"
#include "StatusBandConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @brief Writes, in the zero color, the zeros that make @p digits
 * STATUS_SCORE_DIGITS long, ending where the digits of @p score start. */
void paintLeadingZeros(IInterfaceCanvas& canvas, const PaddedScore& score,
                       std::string_view digits) {
  const int missingDigits =
      STATUS_SCORE_DIGITS - static_cast<int>(digits.size());
  if (missingDigits <= 0) {
    return;
  }
  canvas.write(
      std::string(static_cast<std::size_t>(missingDigits), LEADING_ZERO),
      {score.end.x - (regularTextWidth(digits) * score.scale), score.end.y},
      {.font = TextFont::Regular,
       .color = score.zeroColor,
       .alignment = TextAlignment::Right,
       .scale = score.scale});
}

}  // namespace

void paintPaddedScore(IInterfaceCanvas& canvas, const PaddedScore& score) {
  const std::string digits =
      std::to_string(std::min(score.value, LARGEST_SHOWN_SCORE));
  canvas.write(digits, score.end,
               {.font = TextFont::Regular,
                .color = score.digitColor,
                .alignment = TextAlignment::Right,
                .scale = score.scale});
  paintLeadingZeros(canvas, score, digits);
}

}  // namespace rtype::client
