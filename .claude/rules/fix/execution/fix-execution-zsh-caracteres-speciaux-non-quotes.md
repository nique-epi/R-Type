---
description: En zsh, ?, *, [, {, ^, # et ~ non quotés sont expansés, et un = en tête de mot aussi — un échec d'expansion annule toute la ligne
trigger: always_on
---

# RULE : En zsh, `?`, `*`, `[` et un `=` en tête de mot non quotés sont EXPANSÉS — et un échec d'expansion arrête toute la ligne

## Règle à appliquer

1. **Tout argument qui contient `?`, `*`, `[`, `]`, `{`, `}`, `^`, `#` ou `~` sans être un glob voulu se met entre quotes simples** : URL avec query string, `gh api '…?per_page=100'`, filtres `jq` / `--jq` avec crochets, regex, pathspec git (`git ls-files '*.cpp'`).
2. **Ne jamais commencer un mot non quoté par `=`** : pour un séparateur, `echo '-----'`.
3. **`no matches found` ou `<mot> not found` en zsh veut dire que la commande n'a PAS tourné.** Ne pas lire sa sortie vide comme un résultat : corriger le quoting et relancer.

## Exemple

- ❌ **Avant (incorrect)** : `gh api repos/nique-epi/R-Type/pulls?state=all --jq .[].title; echo =====` → `no matches found`, puis `===== not found` : rien ne s'exécute.
- ✅ **Après (correct)** : `gh api 'repos/nique-epi/R-Type/pulls?state=all' --jq '.[].title'; echo '-----'`.
