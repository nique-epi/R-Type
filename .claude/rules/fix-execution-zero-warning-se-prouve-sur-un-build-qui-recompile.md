---
description: « Zéro warning » ne se prouve que sur la sortie du build qui a réellement recompilé les fichiers touchés, sur les compilateurs de la CI
trigger: always_on
---

# RULE : « Zéro warning » ne se prouve que sur un build qui RECOMPILE les fichiers touchés

## Règle à appliquer

1. **Compter les warnings sur LA sortie du build qui a compilé les fichiers touchés** — le premier build après l'édition, jamais un re-run incrémental qui ne compile rien. Capturer la sortie complète dans un fichier, puis la filtrer :
   ```bash
   cmake --build build > /tmp/build.log 2>&1; echo "exit: $?"
   grep -E "warning|error" /tmp/build.log
   ```
2. **Si un re-run est nécessaire, prouver qu'il a recompilé** : la sortie doit contenir les lignes de compilation des fichiers édités (`Building CXX object …`). Sinon, `cmake --build build --clean-first`.
3. **Un build local vert ne couvre que son compilateur.** La CI compile avec GCC (Linux) et MSVC (Windows), avec `CMAKE_COMPILE_WARNING_AS_ERROR=ON` : un warning MSVC (conversions `size_t` → `int`, `C4267`, `C4244`) n'apparaît pas avec AppleClang ou GCC. Annoncer « zéro warning » en précisant le compilateur, et lire la CI pour les autres.
4. Les options de warnings du projet sont celles de `rtype_enable_warnings()` ; toute nouvelle cible l'appelle.

## Exemple

- ❌ **Avant (incorrect)** : build vert → second `cmake --build build | grep -c warning` → `0` (rien recompilé) → « zéro warning ».
- ✅ **Après (correct)** : log du build qui suit l'édition → `grep -E "warning|error"` vide → « zéro warning sous AppleClang ; GCC et MSVC vérifiés par la CI de la PR ».
