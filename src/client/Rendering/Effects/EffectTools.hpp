#pragma once

#include <functional>

namespace rtype::engine {

class ComponentRegistry;
class EntityRegistry;

}  // namespace rtype::engine

namespace rtype::client {

class CameraShake;
class FlashGate;
class PixelEffects;
struct PilotShades;

/**
 * @brief What every family of effects plays with: the registries the effects
 * are entities of, the gate of the flash preference, the pixel effects, the
 * camera shake, and the shades pilots are painted in. EffectSpawner owns them
 * and hands each family the same tools, which must outlive the families.
 */
struct EffectTools {
  std::reference_wrapper<engine::EntityRegistry> entities;
  std::reference_wrapper<engine::ComponentRegistry> components;
  std::reference_wrapper<FlashGate> gate;
  std::reference_wrapper<PixelEffects> pixels;
  std::reference_wrapper<CameraShake> camera;
  std::reference_wrapper<const PilotShades> shades;
};

}  // namespace rtype::client
