---
description: Commentaires — anglais, Doxygen au-dessus des déclarations, aucun commentaire dans le corps des fonctions, zéro référence de ticket
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE : Commentaires en anglais, au-dessus des déclarations, jamais dans le corps, sans référence de ticket

## Objectif

Un commentaire dans une fonction décrit ce que fait le code d'à côté — ce que le code exprime déjà quand les noms sont bons (cf. `code-style-noms-complets-sans-abreviation.md`). Quand le code change, le commentaire pourrit et finit par mentir. La documentation Doxygen au-dessus des déclarations est extraite par l'outillage (clangd, IDE) et décrit le **contrat**, qui change beaucoup moins que l'implémentation.

## Règles

### Où

- **Aucun commentaire dans le corps d'une fonction, méthode ou lambda** : ni `//`, ni `/* */`, ni hint de paramètre (`/*isReliable=*/true`).
- Une étape qui réclame une explication réclame en réalité un **nom** : extraire une fonction ou renommer une variable.
- Ce qui vaut pour tout le corps (piège, invariant, ordre imposé) remonte dans le doc-comment de la fonction.
- La documentation vit **au-dessus des déclarations**, dans les `.hpp`, en **Doxygen** (`/** @brief … @param … @returns … */`). On documente les types et les membres publics non triviaux ; pas de doc qui répète la signature.

### Exceptions

- Directives d'outillage : `// NOLINTNEXTLINE(check)`, `// NOLINTBEGIN` / `// NOLINTEND`, `// clang-format off` / `on`.
- En-tête de fichier imposé par la norme Epitech, s'il est requis.
- Doxygen au-dessus d'un helper interne (namespace anonyme) réutilisé dans le fichier et dont le contrat mérite d'être écrit.

### Quoi

- **Anglais uniquement.** (Les `.claude/rules/` restent en français : c'est de la doc projet, pas du code.)
- Le **pourquoi** non évident (contrainte, invariant), jamais le **quoi**.
- **Zéro référence à un artefact de planification** — ni ID de ticket, d'issue, de tâche, de finding de revue — dans les commentaires, les noms de variables, les noms de tests ou les messages. Un invariant s'explique en clair, sans l'ID.
- Pas de séparateurs décoratifs, pas de `TODO` orphelin, pas de code commenté.

## Exemples

- ❌ **Interdit** :
  ```cpp
  void MovementSystem::update(Registry &registry, float deltaTime) {
    // Move every entity that has a position and a velocity (TASK-14)
    for (auto [entity, position, velocity] : registry.view<Position, Velocity>()) {
      // on applique la vitesse
      position.x += velocity.x * deltaTime;
      position.y += velocity.y * deltaTime;
    }
  }

  send(packet, /*isReliable=*/true);
  ```
- ✅ **À la place** :
  ```cpp
  /**
   * @brief Advances every entity that has both a Position and a Velocity.
   *
   * @param[in,out] registry  Registry holding the components to update.
   * @param[in]     deltaTime Elapsed time since the previous tick, in seconds.
   */
  void update(Registry &registry, float deltaTime) override;
  ```
  ```cpp
  void MovementSystem::update(Registry &registry, float deltaTime) {
    for (auto [entity, position, velocity] : registry.view<Position, Velocity>()) {
      position.x += velocity.x * deltaTime;
      position.y += velocity.y * deltaTime;
    }
  }

  send(packet, Delivery::Reliable);
  ```
