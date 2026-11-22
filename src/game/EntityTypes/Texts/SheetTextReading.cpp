#include "SheetTextReading.hpp"
#include <algorithm>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>
#include "ActionKey.hpp"
#include "CollisionBox.hpp"
#include "CollisionConstants.hpp"
#include "ContentName.hpp"
#include "ContentNameConstants.hpp"
#include "DataFilePlace.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "DataFileText.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityTypeConstants.hpp"
#include "GameException.hpp"
#include "HurtBeam.hpp"
#include "HurtBox.hpp"
#include "HurtWindow.hpp"
#include "HurtZone.hpp"
#include "MaskConstants.hpp"
#include "MaskSheet.hpp"
#include "ObstacleContact.hpp"
#include "PathText.hpp"
#include "PixelMask.hpp"
#include "SheetActionReading.hpp"
#include "SheetMaskText.hpp"
#include "SheetText.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

namespace {

/** @returns The value of @p key of @p table as a number of logical units. */
float unitsAt(const engine::DataFileTable& table, std::string_view key,
              engine::WholeNumberRange range) {
  return static_cast<float>(engine::wholeNumberAt(table, key, range));
}

/** @returns The value of @p key of @p table in logical units, 0 when the
 * table leaves it out. */
float unitsOrZeroAt(const engine::DataFileTable& table, std::string_view key,
                    engine::WholeNumberRange range) {
  return static_cast<float>(engine::wholeNumberOrZeroAt(table, key, range));
}

/** @returns The box @p table describes: its size, and its offset, 0 without
 * it. @throws engine::UnknownDataFileKeyException for a key other than
 * HITBOX_KEYS. */
CollisionBox boxAt(const engine::DataFileTable& table) {
  engine::refuseUnknownKeys(table, HITBOX_KEYS);
  return CollisionBox{
      .width = unitsAt(table, WIDTH_KEY, HITBOX_SIDE_RANGE),
      .height = unitsAt(table, HEIGHT_KEY, HITBOX_SIDE_RANGE),
      .offsetX = unitsOrZeroAt(table, OFFSET_X_KEY, HURT_BOX_OFFSET_RANGE),
      .offsetY = unitsOrZeroAt(table, OFFSET_Y_KEY, HURT_BOX_OFFSET_RANGE)};
}

/** @throws InconsistentEntitySheetException when @p hitbox, which lists its
 * boxes, gives a size or an offset of its own too. */
void refuseASizeBesideBoxes(const engine::DataFileTable& hitbox) {
  for (const std::string_view key : HITBOX_KEYS) {
    if (hitbox.table.get().contains(key)) {
      throw InconsistentEntitySheetException(engine::placeOfKey(hitbox, key),
                                             HITBOX_SHAPE_RULE);
    }
  }
}

/** @returns The smallest box that holds every box of @p parts, which is not
 * empty. */
CollisionBox enclosingBoxOf(std::span<const CollisionBox> parts) {
  float left = parts.front().offsetX - (parts.front().width * engine::HALF);
  float right = parts.front().offsetX + (parts.front().width * engine::HALF);
  float top = parts.front().offsetY - (parts.front().height * engine::HALF);
  float bottom = parts.front().offsetY + (parts.front().height * engine::HALF);
  for (const CollisionBox& part : parts) {
    left = std::min(left, part.offsetX - (part.width * engine::HALF));
    right = std::max(right, part.offsetX + (part.width * engine::HALF));
    top = std::min(top, part.offsetY - (part.height * engine::HALF));
    bottom = std::max(bottom, part.offsetY + (part.height * engine::HALF));
  }
  return CollisionBox{.width = right - left,
                      .height = bottom - top,
                      .offsetX = (left + right) * engine::HALF,
                      .offsetY = (top + bottom) * engine::HALF};
}

/** @returns The mask file next to the sheet @p file, when there is one.
 * @throws engine::UnreadableDataFileException when it cannot be read. */
std::optional<SheetMaskText> maskTextNextTo(const std::filesystem::path& file) {
  const std::filesystem::path mask =
      file.parent_path() / maskPathOf(logging::genericText(file.stem()));
  std::error_code error;
  if (!std::filesystem::is_regular_file(mask, error)) {
    return std::nullopt;
  }
  return SheetMaskText{.source = engine::shownPath(mask),
                       .text = engine::dataFileContent(mask)};
}

/** @brief Reads the sheets @p files, with their mask files, adding them to
 * @p texts. */
void addSheetTexts(std::span<const std::filesystem::path> files,
                   std::vector<SheetText>& texts) {
  for (const std::filesystem::path& file : files) {
    texts.push_back(SheetText{.name = logging::genericText(file.stem()),
                              .source = engine::shownPath(file),
                              .text = engine::dataFileContent(file),
                              .mask = maskTextNextTo(file)});
  }
}

/** @throws DuplicateEntityTypeException when two of @p texts share a name:
 * two content folders hold a sheet of that name. */
void refuseSharedNames(std::span<const SheetText> texts) {
  std::set<std::string> names;
  for (const SheetText& text : texts) {
    if (!names.insert(text.name).second) {
      throw DuplicateEntityTypeException(text.name);
    }
  }
}

/** @returns The sheet folder of @p contentFolder. */
std::filesystem::path sheetFolderOf(
    const std::filesystem::path& contentFolder) {
  return contentFolder / ENTITY_SHEET_FOLDER_IN_ASSETS;
}

/** @throws InconsistentEntitySheetException, at @p file, the first line of
 * the mask file, when @p rows hold no row, a first row of no pixel, or more
 * rows or pixels than a mask may hold. */
void checkMaskSides(const std::vector<engine::DataFileText>& rows,
                    const engine::DataFilePlace& file) {
  const bool fits =
      !rows.empty() &&
      std::cmp_less_equal(rows.size(), engine::MAXIMUM_MASK_SIDE) &&
      !rows.front().text.empty() &&
      std::cmp_less_equal(rows.front().text.size(), engine::MAXIMUM_MASK_SIDE);
  if (!fits) {
    throw InconsistentEntitySheetException(file, MASK_SIDE_RULE);
  }
}

/** @throws InconsistentEntitySheetException, at the row, when @p row is not
 * as wide as @p first or holds another character than MASK_PIXELS. */
void checkMaskRow(const engine::DataFileText& row,
                  const engine::DataFileText& first) {
  if (row.text.size() != first.text.size()) {
    throw InconsistentEntitySheetException(row.place, MASK_ROW_WIDTH_RULE);
  }
  if (row.text.find_first_not_of(MASK_PIXELS) != std::string::npos) {
    throw InconsistentEntitySheetException(row.place, MASK_PIXEL_RULE);
  }
}

/** @throws InconsistentEntitySheetException when @p rows, the rows of the
 * mask file whose first line is @p file, do not draw a mask:
 * checkMaskSides(), checkMaskRow() for each row, then at least one drawn
 * pixel. */
void checkMaskRows(const std::vector<engine::DataFileText>& rows,
                   const engine::DataFilePlace& file) {
  checkMaskSides(rows, file);
  for (const engine::DataFileText& row : rows) {
    checkMaskRow(row, rows.front());
  }
  const bool drawsNothing =
      std::ranges::none_of(rows, [](const engine::DataFileText& row) noexcept {
        return std::ranges::find(row.text, DRAWN_MASK_PIXEL) != row.text.end();
      });
  if (drawsNothing) {
    throw InconsistentEntitySheetException(file, MASK_DRAWN_RULE);
  }
}

/** @returns The rows of @p mask, one per line, each placed at its line of the
 * file under @p key for an error about it; a carriage return ending a line is
 * no part of its row. */
std::vector<engine::DataFileText> rowsOf(const SheetMaskText& mask,
                                         const std::string& key) {
  std::vector<engine::DataFileText> rows;
  std::size_t start = 0;
  while (start < mask.text.size()) {
    const std::size_t end =
        std::min(mask.text.find('\n', start), mask.text.size());
    std::string row = mask.text.substr(start, end - start);
    if (!row.empty() && row.back() == '\r') {
      row.pop_back();
    }
    rows.push_back(engine::DataFileText{
        .text = std::move(row),
        .place =
            engine::DataFilePlace{.file = mask.source,
                                  .line = FIRST_LINE_OF_A_MASK + rows.size(),
                                  .column = FIRST_COLUMN_OF_A_MASK,
                                  .key = key}});
    start = end + 1;
  }
  return rows;
}

/**
 * @returns The mask file of @p sheet, which @p mask names under FILE_KEY.
 * @throws InconsistentEntitySheetException, at FILE_KEY, when it names
 * another file than maskPathOf() the sheet, or the sheet has no mask file.
 */
const SheetMaskText& namedMaskOf(const engine::DataFileTable& mask,
                                 const SheetText& sheet) {
  if (engine::textAt(mask, FILE_KEY) != maskPathOf(sheet.name)) {
    throw InconsistentEntitySheetException(engine::placeOfKey(mask, FILE_KEY),
                                           MASK_FILE_RULE);
  }
  if (!sheet.mask.has_value()) {
    throw InconsistentEntitySheetException(engine::placeOfKey(mask, FILE_KEY),
                                           MASK_FILE_RULE);
  }
  return *sheet.mask;
}

/** @returns The mask @p rows draw, rows checkMaskRows() accepted. */
engine::PixelMask pixelsOf(const std::vector<engine::DataFileText>& rows) {
  const std::size_t width = rows.front().text.size();
  engine::PixelMask mask(static_cast<int>(width),
                         static_cast<int>(rows.size()));
  for (std::size_t line = 0; line < rows.size(); ++line) {
    for (std::size_t column = 0; column < width; ++column) {
      if (rows[line].text[column] == DRAWN_MASK_PIXEL) {
        mask.markDrawn(
            {.x = static_cast<int>(column), .y = static_cast<int>(line)});
      }
    }
  }
  return mask;
}

/** @returns The attack and the phase the box @p box hurts in, nothing for a
 * box that hurts all the time.
 * @throws InconsistentEntitySheetException when it names only one of them. */
std::optional<ActionKey> activeInFrom(const engine::DataFileTable& box) {
  const bool namesAnAttack = box.table.get().contains(ATTACK_KEY);
  const bool namesAPhase = box.table.get().contains(PHASE_KEY);
  if (namesAnAttack != namesAPhase) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(box, namesAnAttack ? ATTACK_KEY : PHASE_KEY),
        HURT_BOX_ACTION_RULE);
  }
  if (!namesAnAttack) {
    return std::nullopt;
  }
  return actionKeyAt(box);
}

/** @returns When @p box hurts within its phase: from `from_ms`, 0 without it,
 * until `until_ms`, the end of the phase without it.
 * @throws InconsistentEntitySheetException when @p box names a time and no
 * phase (@p activeIn empty), or stops hurting no later than it starts. */
HurtWindow windowFrom(const engine::DataFileTable& box,
                      const std::optional<ActionKey>& activeIn) {
  const toml::table& keys = box.table.get();
  if (!activeIn.has_value() &&
      (keys.contains(FROM_KEY) || keys.contains(UNTIL_KEY))) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(box, keys.contains(FROM_KEY) ? FROM_KEY : UNTIL_KEY),
        HURT_BOX_TIMING_RULE);
  }
  HurtWindow window;
  if (keys.contains(FROM_KEY)) {
    window.from = engine::millisecondsAt(box, FROM_KEY, HURT_BOX_TIME_RANGE);
  }
  if (keys.contains(UNTIL_KEY)) {
    window.until = engine::millisecondsAt(box, UNTIL_KEY, HURT_BOX_TIME_RANGE);
  }
  if (window.until.has_value() && *window.until <= window.from) {
    throw InconsistentEntitySheetException(engine::placeOfKey(box, UNTIL_KEY),
                                           HURT_BOX_WINDOW_RULE);
  }
  return window;
}

/** @returns The beam one table of `[[gameplay.hurt_zone.beam]]` describes.
 * @throws InconsistentEntitySheetException when it sweeps with no end. */
HurtBeam hurtBeamFrom(const engine::DataFileTable& beam) {
  engine::refuseUnknownKeys(beam, HURT_BEAM_KEYS);
  const ActionKey activeIn = actionKeyAt(beam);
  const HurtBeam read{
      .offsetX = unitsAt(beam, OFFSET_X_KEY, HURT_BOX_OFFSET_RANGE),
      .offsetY = unitsAt(beam, OFFSET_Y_KEY, HURT_BOX_OFFSET_RANGE),
      .length = unitsAt(beam, LENGTH_KEY, BEAM_LENGTH_RANGE),
      .thickness = unitsAt(beam, THICKNESS_KEY, HITBOX_SIDE_RANGE),
      .fromDegrees = unitsAt(beam, FROM_DEGREES_KEY, BEAM_ANGLE_RANGE),
      .toDegrees = unitsAt(beam, TO_DEGREES_KEY, BEAM_ANGLE_RANGE),
      .activeIn = activeIn,
      .window = windowFrom(beam, activeIn)};
  if (read.fromDegrees != read.toDegrees && !read.window.until.has_value()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(beam, TO_DEGREES_KEY), HURT_BEAM_SWEEP_RULE);
  }
  return read;
}

/** @returns The box one table of `[[gameplay.hurt_zone.box]]` describes. */
HurtBox hurtBoxFrom(const engine::DataFileTable& box) {
  engine::refuseUnknownKeys(box, HURT_BOX_KEYS);
  const std::optional<ActionKey> activeIn = activeInFrom(box);
  return HurtBox{.offsetX = unitsAt(box, OFFSET_X_KEY, HURT_BOX_OFFSET_RANGE),
                 .offsetY = unitsAt(box, OFFSET_Y_KEY, HURT_BOX_OFFSET_RANGE),
                 .width = unitsAt(box, WIDTH_KEY, HITBOX_SIDE_RANGE),
                 .height = unitsAt(box, HEIGHT_KEY, HITBOX_SIDE_RANGE),
                 .activeIn = activeIn,
                 .window = windowFrom(box, activeIn)};
}

}  // namespace

std::vector<SheetText> sheetTextsIn(const std::filesystem::path& folder) {
  const std::vector<std::filesystem::path> files = engine::dataFilesIn(folder);
  if (files.empty()) {
    throw NoEntitySheetException(engine::shownPath(folder));
  }
  std::vector<SheetText> texts;
  texts.reserve(files.size());
  addSheetTexts(files, texts);
  checkSheetNames(texts);
  return texts;
}

std::vector<SheetText> sheetTextsInFolders(
    std::span<const std::filesystem::path> contentFolders) {
  if (contentFolders.empty()) {
    throw NoEntitySheetException(std::string{});
  }
  std::vector<SheetText> texts =
      sheetTextsIn(sheetFolderOf(contentFolders.front()));
  for (const std::filesystem::path& folder : contentFolders.subspan(1)) {
    const std::filesystem::path sheets = sheetFolderOf(folder);
    std::error_code error;
    if (std::filesystem::is_directory(sheets, error)) {
      addSheetTexts(engine::dataFilesIn(sheets), texts);
    }
  }
  checkSheetNames(texts);
  refuseSharedNames(texts);
  return texts;
}

void checkSheetNames(std::span<const SheetText> texts) {
  if (texts.size() > identifiers::MAXIMUM_ENTITY_TYPES) {
    throw TooManyEntityTypesException(texts.size(),
                                      identifiers::MAXIMUM_ENTITY_TYPES);
  }
  for (const SheetText& text : texts) {
    if (!identifiers::isContentName(text.name)) {
      throw InvalidEntityTypeNameException(
          text.source, identifiers::MAXIMUM_CONTENT_NAME_LENGTH);
    }
  }
}

engine::DataFileTable sheetTableOf(const engine::DataFileTable& root,
                                   std::string_view key) {
  engine::refuseUnknownKeys(root, SHEET_ROOT_KEYS);
  for (const std::string_view other : SHEET_ROOT_KEYS) {
    if (other != key) {
      static_cast<void>(engine::tableAt(root, other));
    }
  }
  return engine::tableAt(root, key);
}

std::vector<CollisionBox> hitboxPartsOf(const engine::DataFileTable& gameplay) {
  const engine::DataFileTable hitbox = engine::tableAt(gameplay, HITBOX_KEY);
  if (!hitbox.table.get().contains(BOX_KEY)) {
    return {};
  }
  engine::refuseUnknownKeys(hitbox, HITBOX_TABLE_KEYS);
  refuseASizeBesideBoxes(hitbox);
  std::vector<CollisionBox> parts;
  for (const engine::DataFileTable& box : engine::tablesAt(hitbox, BOX_KEY)) {
    parts.push_back(boxAt(box));
  }
  if (parts.empty()) {
    throw InconsistentEntitySheetException(engine::placeOfKey(hitbox, BOX_KEY),
                                           HITBOX_BOXES_RULE);
  }
  return parts;
}

CollisionBox hitboxOf(const engine::DataFileTable& gameplay) {
  const std::vector<CollisionBox> parts = hitboxPartsOf(gameplay);
  if (!parts.empty()) {
    return enclosingBoxOf(parts);
  }
  return boxAt(engine::tableAt(gameplay, HITBOX_KEY));
}

std::optional<ObstacleContact> obstacleContactOf(
    const engine::DataFileTable& gameplay) {
  const std::optional<engine::DataFileTable> obstacle =
      engine::optionalTableAt(gameplay, OBSTACLE_KEY);
  if (!obstacle.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*obstacle, OBSTACLE_KEYS);
  return CONTACTS.at(engine::choiceAt(*obstacle, CONTACT_KEY, CONTACT_NAMES));
}

std::string maskPathOf(std::string_view sheetName) {
  return std::string{MASK_FOLDER_OF_SHEETS} + ASSET_PATH_SEPARATOR +
         std::string{sheetName} + std::string{MASK_FILE_EXTENSION};
}

std::optional<MaskSheet> maskOf(const engine::DataFileTable& gameplay,
                                const SheetText& sheet) {
  const std::optional<engine::DataFileTable> mask =
      engine::optionalTableAt(gameplay, MASK_KEY);
  if (!mask.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*mask, MASK_KEYS);
  const SheetMaskText& file = namedMaskOf(*mask, sheet);
  const std::string key = engine::placeOfKey(*mask, FILE_KEY).key;
  const std::vector<engine::DataFileText> rows = rowsOf(file, key);
  checkMaskRows(rows, engine::DataFilePlace{.file = file.source,
                                            .line = FIRST_LINE_OF_A_MASK,
                                            .column = FIRST_COLUMN_OF_A_MASK,
                                            .key = key});
  return MaskSheet{
      .pixels = std::make_shared<const engine::PixelMask>(pixelsOf(rows)),
      .erosion = static_cast<int>(
          engine::wholeNumberOrZeroAt(*mask, EROSION_KEY, MASK_EROSION_RANGE))};
}

std::optional<HurtZone> hurtZoneOf(const engine::DataFileTable& gameplay) {
  const std::optional<engine::DataFileTable> zone =
      engine::optionalTableAt(gameplay, HURT_ZONE_KEY);
  if (!zone.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*zone, HURT_ZONE_KEYS);
  HurtZone read;
  for (const engine::DataFileTable& box : engine::tablesAt(*zone, BOX_KEY)) {
    read.boxes.push_back(hurtBoxFrom(box));
  }
  if (read.boxes.empty()) {
    throw InconsistentEntitySheetException(engine::placeOfKey(*zone, BOX_KEY),
                                           HURT_ZONE_BOXES_RULE);
  }
  if (zone->table.get().contains(BEAM_KEY)) {
    for (const engine::DataFileTable& beam :
         engine::tablesAt(*zone, BEAM_KEY)) {
      read.beams.push_back(hurtBeamFrom(beam));
    }
  }
  return read;
}

}  // namespace rtype::game
