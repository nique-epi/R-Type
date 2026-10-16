#pragma once

#include <cstdint>

namespace rtype::identifiers {

/**
 * @brief The entity id no entity has. The server never gives it to an entity, a
 * message that names no entity carries it, and a value-initialized
 * NetworkIdentity (NetworkIdentity{}) holds it.
 *
 * The engine, which hands out entity ids and finds the entity of each, and the
 * protocol, which carries them, both read it. It lives in a library of its own
 * because rtype_network may not link rtype_engine.
 */
constexpr std::uint32_t NO_ENTITY_ID = 0;

}  // namespace rtype::identifiers
