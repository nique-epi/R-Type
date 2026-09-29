---
description: Vérifier l'artefact livré lui-même (binaire, run de CI, état de la PR), jamais une empreinte indirecte ; ne pas inventer d'explication pour sauver une affirmation
trigger: always_on
---

# RULE : Vérifier l'artefact livré lui-même, jamais une empreinte indirecte

## Règle à appliquer

Quand une conclusion porte sur **ce qui a été livré** (un binaire, un run de CI, une PR, une branche distante) :

1. **Ouvrir l'artefact lui-même.** Un binaire se vérifie en le lançant (et `ls -l` sur le fichier produit à la racine du dépôt), un run de CI par son log (`gh run view <id> --log-failed`), une PR par son état (`gh pr view <n> --json state,headRefOid,mergeable`). Jamais déduit d'un dossier intermédiaire ou d'un code de retour.
2. **Ne jamais tirer une preuve d'absence d'un seul emplacement** sans avoir listé où la chose peut légitimement se trouver (racine du dépôt, `build/`, `Release/` avec le générateur Visual Studio).
3. **Quand l'observation contredit l'utilisateur, ne pas inventer le pont** : le dire (« ce que je vois contredit X »), puis chercher la mesure qui tranche.
4. **Après toute opération sur une branche qui porte une PR ouverte** (force-push, rebase, renommage), vérifier l'état de la PR. Ne jamais renommer ni supprimer une branche avec une PR ouverte.
5. **Avant d'attribuer une écriture à un outil** (clang-format, CMake, vcpkg), prouver qui a écrit : `git log -- <fichier>`.
6. **L'index d'un outil n'est pas le disque** : avant d'affirmer qu'un espace est purgé, lister le support lui-même (`git worktree list` **et** le dossier, `git branch` **et** `git ls-remote`).

## Exemple

- ❌ **Avant (incorrect)** : « la CI Windows passe » parce que le job Linux est vert et que le code n'a rien de spécifique.
- ✅ **Après (correct)** : `gh pr checks <n>` → le job Windows est en échec → `gh run view <id> --log-failed` → avertissement MSVC `C4267` traité en erreur → correctif ciblé.
