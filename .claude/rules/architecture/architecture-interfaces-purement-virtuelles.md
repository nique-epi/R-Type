---
description: Interfaces C++ — uniquement des méthodes pures (= 0) et un destructeur virtuel = default
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE : Une interface n'expose que des méthodes purement virtuelles

## Objectif

Une interface décrit un **contrat**, pas un comportement. Lui donner une implémentation par défaut (`virtual void update() {}`) brouille la frontière entre interface et classe abstraite, et masque les oublis d'implémentation dans les classes dérivées. C'est d'autant plus important sur R-Type que les frontières du moteur (rendu, audio, réseau, entrées) passent par des interfaces.

## Règles

- Une **interface** (classe destinée uniquement à être implémentée, préfixée `I` : `IRenderer`, `ISystem`, `INetworkClient`) n'a que :
  - des méthodes **purement virtuelles** (`= 0`) ;
  - un **destructeur virtuel `= default`**, seule « implémentation » tolérée.
- **Aucune méthode d'interface n'a de corps `{}`**, même vide.
- Pas de membre de donnée dans une interface.
- Une classe abstraite **partiellement** implémentée (état partagé, méthodes communes) n'est pas une interface : elle ne porte pas le préfixe `I` et n'est pas concernée par cette règle.

## Exemples

- ❌ **Interdit** :
  ```cpp
  class ISystem {
   public:
    virtual ~ISystem() = default;
    virtual void update(float deltaTime) {}
    virtual void reset() {}
  };
  ```
- ✅ **À la place** :
  ```cpp
  // ISystem.hpp
  class ISystem {
   public:
    virtual ~ISystem() = default;
    virtual void update(float deltaTime) = 0;
    virtual void reset() = 0;
  };
  ```
