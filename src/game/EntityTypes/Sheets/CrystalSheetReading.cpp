#include "CrystalSheetReading.hpp"
#include <optional>
#include <vector>
#include "CrystalDrop.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "DataFileText.hpp"
#include "EntitySheetConstants.hpp"

namespace rtype::game {

std::optional<engine::DataFileText> crystalFrom(
    const engine::DataFileTable& gameplay) {
  if (!gameplay.table.get().contains(CRYSTAL_KEY)) {
    return std::nullopt;
  }
  return engine::DataFileText{
      .text = engine::textAt(gameplay, CRYSTAL_KEY),
      .place = engine::placeOfKey(gameplay, CRYSTAL_KEY)};
}

std::vector<CrystalDrop> dropsFrom(const engine::DataFileTable& gameplay) {
  if (!gameplay.table.get().contains(DROPS_KEY)) {
    return {};
  }
  std::vector<CrystalDrop> drops;
  for (const engine::DataFileText& named :
       engine::textsAt(gameplay, DROPS_KEY)) {
    drops.push_back(
        CrystalDrop{.name = named.text, .place = named.place, .number = 0});
  }
  return drops;
}

}  // namespace rtype::game
