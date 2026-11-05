#pragma once

#include <string_view>
#include "CollisionBox.hpp"
#include "EntityAppearances.hpp"
#include "EntitySheet.hpp"
#include "HurtBox.hpp"
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

/** @returns Whether @p frame, laid as holds() lays it, holds the hurt box
 * @p box, offset from the body. */
[[nodiscard]] bool holds(rtype::game::SpriteFrame frame,
                         const rtype::game::HurtBox& box);
