---
description: Pull requests — intégralement en anglais, titre Conventional Commits, description calquée sur la structure des PR du dépôt
trigger: always_on
---

# RULE : Une pull request s'écrit en ANGLAIS et suit la structure des PR du dépôt

## Règle à appliquer

1. **Toute pull request s'écrit en anglais** : titre, description entière, en-têtes de sections, tableaux, listes, et **tout commentaire posté sur la PR** (revue, réponse, suivi). Aucun mélange de langues dans un même document. La langue de la conversation n'a aucune incidence sur ce qui est écrit dans le dépôt.
2. **Le titre suit Conventional Commits**, comme les commits (cf. `commit.md`).
3. **La description suit la structure des PR du dépôt**, toutes les sections, dans l'ordre :
   `## Summary` (avec `Closes #` si une issue existe) · `## Changes` · `## Type of Change` · `## Testing` · `## Checklist`.
   - Si `.github/PULL_REQUEST_TEMPLATE.md` existe, **c'est lui qui fait foi** : le lire avant d'ouvrir ou d'éditer la PR.
   - Sinon, calquer la dernière PR mergée (`gh pr view <n> --json body`).
   - Les cases de `Type of Change` et `Checklist` se cochent **honnêtement** : uniquement ce qui est vrai et vérifié.
4. **`## Testing` donne les commandes réellement lancées et leur résultat** (plateforme, compilateur, sortie), jamais une intention.
5. **Une PR empilée sur une autre le dit** en tête de Summary (base, et quand la retarget sur `main`).
6. **Les noms propres et identifiants ne se traduisent pas** : fichiers, branches, cibles CMake, symboles.
7. **Aucune mention d'IA** dans le titre ou la description (cf. `fix-process-pas-de-co-author-commit.md`).
8. **Relecture avant envoi** : si le titre ou la première section est en français, la PR n'est pas prête.

## Exemple

- ❌ **Avant (incorrect)** :
  ```md
  ## Résumé
  Cette PR ajoute le parseur de paquets.

  ## Modifications
  - ...
  ```
- ✅ **Après (correct)** :
  ```md
  ## Summary
  Adds the packet reader that turns a received datagram into a typed message.

  Closes #

  ## Changes
  - ...

  ## Type of Change
  - [ ] Bug fix
  - [x] New feature
  - [ ] Refactor
  - [ ] Documentation

  ## Testing
  macOS arm64 (AppleClang): `cmake --workflow --preset test` -> 14/14 passing, no warnings.

  ## Checklist
  - [x] Code compiles without errors
  - [x] No new warnings introduced
  - [x] Tests pass
  - [x] Self-reviewed the diff
  ```
