---
description: Vérifier l'index avant de committer et le commit produit après — un git add qui échoue ne stage rien
trigger: always_on
---

# RULE : Vérifier l'index AVANT de committer — un `git add` qui échoue ne stage rien du tout

## Règle à appliquer

1. **Un `git add` avec un chemin invalide échoue pour tous les chemins** : `pathspec did not match` = rien n'est stagé. Ne jamais ré-`add` un chemin déjà supprimé par `git rm` ; pour ratisser large, `git add -A -- <dossiers>`.
2. **Vérifier l'index avant de committer**, systématiquement :
   ```bash
   git diff --cached --no-renames --name-status
   ```
   La liste doit correspondre exactement à l'intention (compare des chemins, pas un compte). Un `add` et un `commit` ne se chaînent pas à l'aveugle.
3. **Vérifier le commit produit** juste après : `git show --stat --format= HEAD`.
4. **Ne jamais changer de branche avec des modifications non commitées** qui appartiennent à la branche courante : `git status --porcelain` d'abord, sinon les modifications voyagent en silence sur la branche d'à côté.
5. **Ne jamais committer d'artefact de build** : `build/`, binaires `r-type_server` / `r-type_client` à la racine, `compile_commands.json`, `vcpkg_installed/`. Si l'un apparaît dans `git status`, c'est le `.gitignore` qui est à corriger.
6. **`git checkout -- <fichier>` détruit** les modifications non commitées du fichier, sans filet.

## Exemple

- ❌ **Avant (incorrect)** : `git rm old.cpp && git add old.cpp new.cpp CMakeLists.txt && git commit -m "…"` → `pathspec 'old.cpp'` → commit ne contenant que la suppression.
- ✅ **Après (correct)** : `git rm old.cpp` → `git add -- new.cpp CMakeLists.txt` → `git diff --cached --no-renames --name-status` (D, A, M attendus) → `git commit` → `git show --stat`.
