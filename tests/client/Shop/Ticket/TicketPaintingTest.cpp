#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <string_view>
#include "LookItemDescriptionConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLooks.hpp"
#include "TextStyle.hpp"
#include "TicketPainting.hpp"
#include "TicketTestConstants.hpp"
#include "TicketView.hpp"
#include "WidgetText.hpp"

using rtype::client::LOOK_ITEM_DESCRIPTIONS;
using rtype::client::lookItemLabelOf;
using rtype::client::paintTicket;
using rtype::client::regularTextWidth;
using rtype::client::TextAlignment;
using rtype::client::TextFont;
using rtype::client::TicketDescription;
using rtype::client::TicketView;
using rtype::client::testing::BUY_WORD;
using rtype::client::testing::CHAPKA_DESCRIPTION;
using rtype::client::testing::CHAPKA_ITEM;
using rtype::client::testing::CHAPKA_NAME;
using rtype::client::testing::CHAPKA_NAME_ANCHOR;
using rtype::client::testing::CHAPKA_SCALE;
using rtype::client::testing::CHAPKA_TOP_LEFT;
using rtype::client::testing::HULLS_SHEET;
using rtype::client::testing::LOOKS_SHEET;
using rtype::client::testing::PAID_STAMP_ON_SCREEN;
using rtype::client::testing::PAID_STAMP_SOURCE;
using rtype::client::testing::TICKET_MIDDLE_ON_SCREEN;
using rtype::client::testing::TICKET_SHEET_ON_TICKET;
using rtype::client::testing::TICKET_TEXT_WIDTH_ON_PAPER;
using rtype::client::testing::TYPED_INK;
using rtype::client::testing::WAR_SHARK_ITEM;
using rtype::client::testing::WAR_SHARK_SCALE;
using rtype::client::testing::WAR_SHARK_TOP_LEFT;
using rtype::client::testing::WARE_AREA_ON_SCREEN;
using rtype::identifiers::SHIP_LOOK_ITEM_COUNT;

/**
 * Given the ticket of the chapka, 14 by 9 pixels
 * When it is painted
 * Then the chapka is drawn three times as large at (219, 75), its middle on
 * column 240, the column its name is centred on, at (240, 115)
 */
TEST(TicketPaintingTest, TheWareIsCentredOverItsName) {
  InterfaceCanvasRecord painting;
  RecordingInterfaceCanvas canvas(painting);

  paintTicket(canvas, TicketView{.item = CHAPKA_ITEM});

  const auto icon =
      std::ranges::find(painting.images, LOOKS_SHEET, &RecordedImage::assetId);
  ASSERT_NE(icon, painting.images.end());
  EXPECT_EQ(icon->topLeft, CHAPKA_TOP_LEFT);
  EXPECT_EQ(icon->scale, CHAPKA_SCALE);
  EXPECT_EQ(icon->topLeft.x + ((icon->source.size.x * icon->scale) / 2),
            TICKET_MIDDLE_ON_SCREEN);
  const auto* name = writingOf(painting, CHAPKA_NAME);
  ASSERT_NE(name, nullptr);
  EXPECT_EQ(name->anchor, CHAPKA_NAME_ANCHOR);
  EXPECT_EQ(name->style.alignment, TextAlignment::Center);
}

/**
 * Given the ticket of the chapka
 * When it is painted
 * Then each line of its description is typed in the regular font, the bold
 * one, in the dark ink, centred
 */
TEST(TicketPaintingTest, TheDescriptionIsTypedInBoldDarkInk) {
  InterfaceCanvasRecord painting;
  RecordingInterfaceCanvas canvas(painting);

  paintTicket(canvas, TicketView{.item = CHAPKA_ITEM});

  for (const std::string_view line : CHAPKA_DESCRIPTION) {
    const auto* written = writingOf(painting, line);
    ASSERT_NE(written, nullptr) << line;
    EXPECT_EQ(written->style.font, TextFont::Regular) << line;
    EXPECT_EQ(written->style.color, TYPED_INK) << line;
    EXPECT_EQ(written->anchor.x, TICKET_MIDDLE_ON_SCREEN) << line;
  }
}

/**
 * Given the ticket of the war shark, a hull of 32 by 16 pixels
 * When it is painted
 * Then its ship is drawn twice as large at (208, 73), centred in the ware
 * area and cut to it
 */
TEST(TicketPaintingTest, AHullIsDrawnCentredInTheWareArea) {
  InterfaceCanvasRecord painting;
  RecordingInterfaceCanvas canvas(painting);

  paintTicket(canvas, TicketView{.item = WAR_SHARK_ITEM});

  const auto ship =
      std::ranges::find(painting.images, HULLS_SHEET, &RecordedImage::assetId);
  ASSERT_NE(ship, painting.images.end());
  EXPECT_EQ(ship->topLeft, WAR_SHARK_TOP_LEFT);
  EXPECT_EQ(ship->scale, WAR_SHARK_SCALE);
  EXPECT_EQ(ship->visibleArea, WARE_AREA_ON_SCREEN);
}

/**
 * Given a paid ticket
 * When it is painted
 * Then its PAID stamp is laid at (196, 180) and its buttons are gone
 */
TEST(TicketPaintingTest, APaidTicketShowsItsStampInsteadOfItsButtons) {
  InterfaceCanvasRecord painting;
  RecordingInterfaceCanvas canvas(painting);

  paintTicket(canvas, TicketView{.item = CHAPKA_ITEM, .paid = true});

  const auto stamp = std::ranges::find_if(
      painting.images, [](const RecordedImage& image) noexcept {
        return image.assetId == TICKET_SHEET_ON_TICKET &&
               image.source == PAID_STAMP_SOURCE;
      });
  ASSERT_NE(stamp, painting.images.end());
  EXPECT_EQ(stamp->topLeft, PAID_STAMP_ON_SCREEN);
  EXPECT_EQ(writingOf(painting, BUY_WORD), nullptr);
}

/**
 * Given the name and the description of every ware of the catalogue
 * When their widths are measured in the regular font
 * Then each fits the 152 pixels of text of the ticket, and each ware has a
 * description
 */
TEST(TicketPaintingTest, EveryWareFitsItsTicket) {
  for (std::size_t item = 0; item < SHIP_LOOK_ITEM_COUNT; ++item) {
    const TicketDescription& description = LOOK_ITEM_DESCRIPTIONS.at(item);
    EXPECT_FALSE(description.front().empty()) << item;
    EXPECT_LE(regularTextWidth(lookItemLabelOf(item)),
              TICKET_TEXT_WIDTH_ON_PAPER)
        << item;
    for (const std::string_view line : description) {
      EXPECT_LE(regularTextWidth(line), TICKET_TEXT_WIDTH_ON_PAPER) << line;
    }
  }
}
