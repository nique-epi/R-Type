#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "CollisionBox.hpp"
#include "EntityAppearance.hpp"
#include "EntityAppearances.hpp"
#include "EntitySheet.hpp"
#include "HurtBox.hpp"
#include "PixelMask.hpp"
#include "SpriteFrame.hpp"

/** @returns The appearances of one sheet, named READ_TYPE_NAME, holding
 * @p text. */
[[nodiscard]] rtype::client::EntityAppearances appearancesReading(
    std::string_view text);

/** @returns The appearances of the sheets of READ_SHEET_TEXT and
 * OTHER_SHEET_TEXT, named READ_TYPE_NAME and UNREAD_TYPE_NAME, and one more
 * sheet drawing from READ_SHEET_ID. */
[[nodiscard]] rtype::client::EntityAppearances threeAppearancesOnTwoSheets();

/** @returns The appearances of the sheets shipped in assets/. */
[[nodiscard]] rtype::client::EntityAppearances shippedAppearances();

/** @returns Whether @p sheet has a hurt box that hurts all the time. */
[[nodiscard]] bool hurtsAllTheTime(const rtype::game::EntitySheet& sheet);

/** @returns Whether @p frame, at BACKGROUND_PIXEL_SIZE logical units a
 * pixel and centered on the body, holds @p box. */
[[nodiscard]] bool holds(rtype::game::SpriteFrame frame,
                         const rtype::game::CollisionBox& box);

/** @returns Whether the hitbox of @p sheet lies away from the center of its
 * body, as the eye of the Bydo Tyrant does. */
[[nodiscard]] bool hitsAnOffsetBox(const rtype::game::EntitySheet& sheet);

/** @returns Whether @p frame, laid as holds() lays it, holds @p box at its
 * offset from the body. */
[[nodiscard]] bool holdsTheHitbox(rtype::game::SpriteFrame frame,
                                  const rtype::game::CollisionBox& box);

/** @returns Whether @p frame, laid as holds() lays it, holds the hurt box
 * @p box, offset from the body. */
[[nodiscard]] bool holds(rtype::game::SpriteFrame frame,
                         const rtype::game::HurtBox& box);

/** @returns The rows of @p mask, from the top: DRAWN_MASK_PIXEL for a drawn
 * pixel and CLEAR_MASK_PIXEL for a clear one, as a sheet writes them. */
[[nodiscard]] std::vector<std::string> rowsOf(
    const rtype::engine::PixelMask& mask);

/** @returns @p rows with @p times pixels of outline peeled: a drawn pixel
 * stays only when its four neighbours are drawn, a neighbour outside the rows
 * counting as clear. */
[[nodiscard]] std::vector<std::string> peeled(std::vector<std::string> rows,
                                              int times);

/** @returns @p rows as texts, one per row, to compare with what peeled() and
 * rowsOf() give. */
[[nodiscard]] std::vector<std::string> rowsFrom(
    std::span<const std::string_view> rows);

/** @returns The rows a mask must hold: the pixels drawn in a frame of the
 * idle or enraged idle animation of @p look, read in its PNG, peeled
 * @p erosion times, then cut to @p hitbox, as
 * tests/tools/collision-masks/write_masks.py writes them. */
[[nodiscard]] std::vector<std::string> expectedMaskRows(
    const rtype::client::EntityAppearance& look, int erosion,
    const rtype::game::CollisionBox& hitbox);
