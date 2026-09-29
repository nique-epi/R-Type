---
description: Un push ne s'affirme jamais sur son code de retour — comparer le SHA distant au HEAD local et à la tête de la PR ; vérifier que HEAD n'est pas détaché avant de committer
trigger: always_on
---

# RULE : Un push ne s'affirme jamais sur son code de retour — comparer le SHA distant au HEAD local

## Règle à appliquer

1. **Avant chaque `git commit`, asserter la branche** : `git branch --show-current` doit rendre la branche attendue. Une sortie **vide** est un HEAD détaché : on ne commit pas, on diagnostique (`git reflog show HEAD -5`) et on se rattache d'abord.
2. **Jamais `-q` sur un push dont on va rapporter le résultat**, jamais `&& echo pushed` : on lit la ligne `old..new  branche -> branche`. « Everything up-to-date » après un commit local est une **erreur**.
3. **Après le push, comparer les SHA** :
   ```bash
   git rev-parse HEAD
   git ls-remote --heads origin <branch> | cut -f1
   gh pr view <n> --json headRefOid --jq .headRefOid
   ```
   Les trois doivent être identiques.
4. **Réparation**, quand les commits détachés sont tous à soi : `git branch -f <branche> HEAD`, `git checkout <branche>`, push verbeux, puis point 3. Si un commit étranger s'y trouve : STOP, demander.

## Exemple

- ❌ **Avant (incorrect)** : `git push -q && echo pushed` → « pushed », PR inchangée, attribuée à « GitHub qui recalcule ».
- ✅ **Après (correct)** : `git branch --show-current` → vide → HEAD détaché → rattacher la branche → push → `ls-remote` = `rev-parse HEAD` = `headRefOid`.
