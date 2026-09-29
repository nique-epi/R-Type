---
description: Séparation stricte .hpp / .cpp — le header déclare, le .cpp définit
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE : Un `.hpp` ne contient que des déclarations, toute définition va dans le `.cpp`

## Objectif

Un `.hpp` décrit l'**interface** d'un composant : il doit pouvoir être survolé en quelques secondes pour comprendre son API. Garder les corps de méthodes hors des headers réduit les temps de compilation, évite les dépendances transitives et sépare clairement contrat et implémentation.

## Règles

- Un `.hpp` contient uniquement : classes, signatures de méthodes, `enum`, `struct`, `using`, déclarations de templates, forward declarations.
- **Aucune logique, aucun corps de méthode** dans un `.hpp` — y compris les getters/setters d'une ligne.
- **Exceptions techniques, limitées et justifiées** :
  - templates qui doivent être définis dans le header (ou dans un `.tpp` inclus en fin de header) ;
  - `constexpr` quand la définition est requise à la compilation ;
  - méthodes triviales `= default` / `= delete`.
- Un header n'inclut que ce dont ses **déclarations** ont besoin ; le reste des `#include` va dans le `.cpp`. Préférer une forward declaration quand un pointeur ou une référence suffit.
- S'applique à tout `.hpp` de `src/` et `tests/`.

## Exemples

- ❌ **Interdit** (`Player.hpp`) :
  ```cpp
  class Player {
   public:
    int health() const { return health_; }
    void takeDamage(int amount) {
      health_ -= amount;
    }

   private:
    int health_;
  };
  ```
- ✅ **À la place** :
  ```cpp
  // Player.hpp
  class Player {
   public:
    int health() const;
    void takeDamage(int amount);

   private:
    int health_;
  };
  ```
  ```cpp
  // Player.cpp
  #include "Player.hpp"

  int Player::health() const { return health_; }

  void Player::takeDamage(int amount) { health_ -= amount; }
  ```
