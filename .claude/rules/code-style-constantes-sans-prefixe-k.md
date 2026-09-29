---
description: Nommage des constantes — pas de préfixe k, lowerCamelCase par défaut, UPPER_SNAKE_CASE toléré pour les constantes publiques d'un header
trigger: always_on
---

# RULE : Pas de préfixe `k` pour les constantes

## Objectif

Le préfixe `k` est une convention Google qui n'apporte rien au-delà de ce que `constexpr`, `const` et `enum` expriment déjà dans le système de types. On garde un nommage direct et uniforme.

## Règles

- Les constantes (`constexpr`, `const`, valeurs d'`enum class`) **ne sont jamais préfixées par `k`**.
- Nom par défaut en `lowerCamelCase` : `tickRate`, `maxPlayers`, `epsilon`.
- `UPPER_SNAKE_CASE` est accepté pour une constante **publique** exposée dans un header et partagée entre modules (`DEFAULT_SERVER_PORT`).
- Valeurs d'`enum class` en `PascalCase` : `PacketType::PlayerInput`.
- Toute constante existante en `kFoo` est renommée dès qu'on touche au fichier.

## Exemples

- ❌ **Interdit** :
  ```cpp
  constexpr int kTickRate = 60;
  constexpr std::size_t kMaxPlayers = 4;
  ```
- ✅ **À la place** :
  ```cpp
  constexpr int tickRate = 60;
  constexpr std::size_t maxPlayers = 4;

  // Public constant shared across modules
  constexpr std::uint16_t DEFAULT_SERVER_PORT = 4242;
  ```
