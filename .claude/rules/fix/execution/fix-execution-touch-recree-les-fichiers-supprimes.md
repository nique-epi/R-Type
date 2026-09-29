---
description: touch sur une liste issue de git status recrée les fichiers supprimés — filtrer sur l'existence, et ne pas toucher aux sources pour forcer un rebuild
trigger: always_on
---

# RULE : `touch` sur une liste de chemins recrée ce qui a été supprimé — filtrer sur ce qui existe

## Règle à appliquer

1. **Ne jamais passer à `touch` une liste issue de `git status` sans filtrer sur l'existence** :
   ```bash
   git diff --name-only --diff-filter=ACMR -z | xargs -0 touch
   ```
2. **Forcer une recompilation se fait sans toucher au working tree** : `cmake --build build --clean-first`, ou un dossier de build neuf. Une preuve de build ne modifie jamais les sources qu'elle mesure.
3. **Un build vert ne prouve rien sur une suppression** : un fichier vide compile, et un fichier `.cpp` supprimé mais toujours listé dans un `CMakeLists.txt` ne se voit qu'à la reconfiguration. C'est l'index (`git diff --cached --name-status`) qui dit ce qui est supprimé.

## Exemple

- ❌ **Avant (incorrect)** : `git status --porcelain | awk '{print $NF}' | xargs touch` → les deux fichiers supprimés réapparaissent vides, build toujours vert.
- ✅ **Après (correct)** : `cmake --build build --clean-first`, puis `git diff --cached --name-status` où les deux `D` attendus sont là.
