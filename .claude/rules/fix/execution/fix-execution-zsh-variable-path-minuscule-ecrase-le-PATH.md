---
description: En zsh, path (minuscules) est l'alias tableau de PATH — ne jamais nommer une variable path, fpath, cdpath…
trigger: always_on
---

# RULE : En zsh, `path` (minuscules) est l'alias tableau de `PATH` — l'affecter détruit le PATH de tout l'appel

## Règle à appliquer

1. **Ne jamais nommer une variable shell `path`, `cdpath`, `fpath`, `manpath`, `module_path`, `watch`, `psvar`** en zsh. Utiliser `file`, `dir`, `target`, `source_file`…
2. **Quand plusieurs commandes de base deviennent « command not found » d'un coup**, ne pas diagnostiquer la machine : chercher d'abord une affectation de `path` (ou un `export PATH=` sans `$PATH`) dans l'appel qui précède.

## Exemple

- ❌ **Avant (incorrect)** : `for path in $(git ls-files '*.hpp'); do clang-format -i "$path"; done` → `command not found: clang-format` dès le premier tour.
- ✅ **Après (correct)** : `git ls-files -z '*.hpp' | xargs -0 clang-format -i`, ou une boucle sur `file`.
