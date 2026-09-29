---
description: Vérifier dans le code qu'une fonctionnalité n'existe pas déjà avant de la classer « à faire »
trigger: always_on
---

# RULE : Vérifier l'état du code avant de scoper

## Règle à appliquer

Avant de classer une fonctionnalité comme « à faire » dans un plan, une issue ou une PR, **vérifier dans le code** qu'elle n'est pas déjà implémentée — et sur les branches ouvertes (`gh pr list`) qu'un coéquipier n'est pas déjà dessus. Ne jamais inférer l'état d'avancement d'une simple formulation de l'utilisateur. En cas de doute, le lever explicitement avant de scoper.

## Exemple

- ❌ **Avant (incorrect)** : planifier « mettre en place la CI » parce que l'utilisateur a mentionné la CI.
- ✅ **Après (correct)** : `gh pr list` → une PR `ci: build and test on Linux and Windows` est ouverte → la CI est en cours, on part de là.
