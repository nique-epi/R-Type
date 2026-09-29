---
description: Tests GoogleTest — un comportement par test, nom descriptif, doc-comment Given / When / Then au-dessus du TEST
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE : Chaque test décrit son scénario en Given / When / Then

## Objectif

Le nom d'un test dit *quoi*, pas *dans quelles conditions* ni *ce qui est attendu*. Le triptyque Given / When / Then rend explicites le pré-état, l'action et le résultat : il sert de mini-spécification, accélère la revue et le diagnostic d'un échec, et pousse à ne valider qu'**un seul** comportement par test.

## Règles

- Tout `TEST` / `TEST_F` / `TEST_P` porte, **au-dessus** de sa déclaration, un doc-comment en trois clauses, une par ligne : `Given` (contexte initial), `When` (action), `Then` (résultat attendu). Il est au-dessus, pas dans le corps (cf. `code-style-commentaires-anglais-hors-du-corps-sans-tickets.md`).
- **Un comportement par test.** Deux `Then` indépendants = deux tests.
- Suite et nom de test en `PascalCase`, décrivant le comportement : `TEST(PacketReader, RejectsPayloadShorterThanHeader)`. Aucun ID de ticket.
- Le `Then` énonce ce que le produit exige, jamais la sortie observée (cf. `fix-process-test-vert-qui-verrouille-un-defaut-et-fake-trop-deterministe.md`).
- Les tests se déclarent via `rtype_add_test()` dans `tests/CMakeLists.txt`.

## Exemples

- ❌ **Interdit** :
  ```cpp
  TEST(Registry, Test1) {
    Registry registry;
    auto entity = registry.createEntity();
    registry.destroyEntity(entity);
    EXPECT_FALSE(registry.isAlive(entity));
    EXPECT_EQ(registry.size(), 0);
  }
  ```
- ✅ **À la place** :
  ```cpp
  /**
   * Given a registry holding a single entity
   * When that entity is destroyed
   * Then the registry no longer reports it as alive
   */
  TEST(Registry, DestroyedEntityIsNoLongerAlive) {
    Registry registry;
    auto entity = registry.createEntity();

    registry.destroyEntity(entity);

    EXPECT_FALSE(registry.isAlive(entity));
  }
  ```
