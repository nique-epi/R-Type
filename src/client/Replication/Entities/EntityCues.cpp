#include "EntityCues.hpp"
#include <optional>
#include <string>
#include <string_view>
#include "AppearanceReference.hpp"
#include "AwaitedArrival.hpp"
#include "ComponentRegistry.hpp"
#include "EffectSpawner.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityFeedback.hpp"
#include "EntitySounds.hpp"
#include "EntityType.hpp"
#include "ISoundPlayer.hpp"
#include "Position.hpp"
#include "SoundConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The sound of the appearance of an entity of that type: the shot
 * of a missile of a player or of an enemy; nothing for a ship or a Bydo. */
std::optional<std::string_view> appearanceSoundOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::PlayerMissile:
      return PLAYER_SHOT_SOUND_ID;
    case game::EntityType::EnemyMissile:
      return ENEMY_SHOT_SOUND_ID;
    case game::EntityType::Player:
    case game::EntityType::Enemy:
    case game::EntityType::LookItem:
    case game::EntityType::Obstacle:
      return std::nullopt;
  }
  return std::nullopt;
}

/** @returns The sound of an entity of that type killed: the big explosion of
 * a ship, the small one of a Bydo; nothing for a missile, which only shows
 * its impact. */
std::optional<std::string_view> explosionSoundOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
      return BIG_EXPLOSION_SOUND_ID;
    case game::EntityType::Enemy:
    case game::EntityType::Obstacle:
      return SMALL_EXPLOSION_SOUND_ID;
    case game::EntityType::PlayerMissile:
    case game::EntityType::EnemyMissile:
    case game::EntityType::LookItem:
      return std::nullopt;
  }
  return std::nullopt;
}

/** @returns The appearance of the type of @p entity, whose sheet may draw
 * its death, or nullptr for an entity created without a sheet. */
const EntityAppearance* appearanceOf(engine::ComponentRegistry& components,
                                     engine::Entity entity) {
  const AppearanceReference* reference =
      components.get<AppearanceReference>(entity);
  return reference == nullptr ? nullptr : reference->appearance;
}

/** @returns The sound the sheet of @p appearance names in @p sound, or
 * @p otherwise when it names none or the entity has no sheet. */
std::optional<std::string_view> sheetSoundOr(
    const EntityAppearance* appearance, std::string EntitySounds::* sound,
    std::optional<std::string_view> otherwise) {
  if (appearance == nullptr || (appearance->sounds.*sound).empty()) {
    return otherwise;
  }
  return appearance->sounds.*sound;
}

/** @brief Plays the sound, when there is one. */
void playSound(ISoundPlayer& sounds, std::optional<std::string_view> soundId) {
  if (soundId.has_value()) {
    sounds.play(*soundId);
  }
}

}  // namespace

void announce(const EntityFeedback& feedback,
              engine::ComponentRegistry& components, engine::Entity entity) {
  const game::EntityType* type = components.get<game::EntityType>(entity);
  const game::Position* position = components.get<game::Position>(entity);
  if (type == nullptr || position == nullptr) {
    return;
  }
  playSound(feedback.sounds,
            sheetSoundOr(appearanceOf(components, entity),
                         &EntitySounds::appear, appearanceSoundOf(*type)));
  if (*type == game::EntityType::Player) {
    feedback.effects.get().playArrival(entity);
    return;
  }
  feedback.effects.get().playAppearance(*type, *position);
}

void explode(const EntityFeedback& feedback,
             engine::ComponentRegistry& components, engine::Entity entity) {
  const game::EntityType* type = components.get<game::EntityType>(entity);
  const game::Position* position = components.get<game::Position>(entity);
  if (type == nullptr || position == nullptr ||
      components.has<AwaitedArrival>(entity)) {
    return;
  }
  playSound(feedback.sounds,
            sheetSoundOr(appearanceOf(components, entity), &EntitySounds::death,
                         explosionSoundOf(*type)));
  if (*type == game::EntityType::Player) {
    feedback.effects.get().playShipDown(entity);
    return;
  }
  feedback.effects.get().playExplosion(*type, *position,
                                       appearanceOf(components, entity));
}

}  // namespace rtype::client
