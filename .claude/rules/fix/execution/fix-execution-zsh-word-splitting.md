---
description: En zsh, une variable ou un $(...) non quoté n'est pas découpé en arguments — passer les listes en tableau ou via find -print0 | xargs -0
trigger: always_on
---

# RULE : En zsh, `$var` et `$(...)` non quotés ne sont PAS découpés en arguments

## Règle à appliquer

1. **Ne jamais passer une liste de fichiers ou d'options via `cmd $var` ou `cmd $(...)` non quoté** : zsh ne découpe pas comme bash, la commande reçoit **un seul** argument géant.
2. **Liste de fichiers** → `find … -print0 | xargs -0 cmd` (gère espaces et retours à la ligne) :
   ```bash
   find src tests -name '*.cpp' -print0 | xargs -0 clang-format --dry-run -Werror
   ```
3. **Liste d'options** → un tableau, passé en tableau :
   ```zsh
   args=(--preset test --output-on-failure)
   ctest "${args[@]}"
   ```
   ou écrite en littéral quand elle est courte.
4. **Variables d'environnement** → affectation inline devant chaque commande (`CC=clang CXX=clang++ cmake …`), jamais `env $VARS cmd`.
5. Si un découpage explicite est vraiment nécessaire : `${(f)var}` (sur les retours à la ligne) ou `${=var}` (sur IFS).
6. **Vérifier l'effet réel** d'une opération de masse (renommage, substitution) par un contrôle chiffré avant de conclure : une passe qui n'édite rien ne produit pas d'erreur.

## Exemple

- ❌ **Avant (incorrect)** : `files=$(git ls-files '*.cpp'); clang-format -i $files` → un seul argument, `No such file or directory`, rien n'est formaté.
- ✅ **Après (correct)** : `git ls-files -z '*.cpp' '*.hpp' | xargs -0 clang-format -i`, puis `git diff --stat` pour constater l'effet.
