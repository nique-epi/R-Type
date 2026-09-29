---
description: Une convention se dérive d'au moins trois instances représentatives, dont les plus récentes — jamais d'un seul exemple
trigger: always_on
---

# RULE : Une convention se dérive d'un échantillon REPRÉSENTATIF (≥ 3, dont les plus récents) — jamais d'un seul exemple

## Règle à appliquer

1. **Avant de codifier ou d'appliquer une convention** (nommage, structure de dossier, format de commit, template de PR, forme d'un système ECS), **lire au moins trois instances complètes**, dont les plus récentes, et vérifier qu'elles convergent. Si elles divergent, la divergence *est* l'information : chercher ce qui distingue les cas avant d'écrire quoi que ce soit.
2. **Une instance isolée qui diffère des autres est une variante à comprendre, jamais la norme.**
3. **Quand l'utilisateur a manifestement un format en tête** (« suis ce qui est déjà en place »), lui **montrer le squelette retenu avant de l'appliquer en masse**.
4. **Ne jamais écrire une affirmation de couverture non mesurée.** « Dérivé de l'historique » exige d'avoir lu l'historique ; sinon on écrit ce qu'on a réellement lu.
5. Voisines : `fix-raisonnement-ne-pas-halluciner-contenu-fichier.md` couvre « je n'ai pas lu » ; `fix-raisonnement-invariant-delegue-doit-etre-calcule.md` couvre « j'ai lu mais j'ai paraphrasé faux » ; celle-ci couvre **« j'ai lu UN cas et je l'ai généralisé »**.

## Exemple

- ❌ **Avant (incorrect)** : un seul commit porte un scope `feat(server): …` → j'impose un scope à tous les commits.
- ✅ **Après (correct)** : `git log --format=%s -20` → la majorité n'a pas de scope → le scope reste facultatif, et l'exception est comprise comme telle.
