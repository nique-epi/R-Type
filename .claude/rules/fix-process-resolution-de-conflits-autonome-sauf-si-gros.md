---
description: Conflits de merge — résoudre seul les conflits mécaniques, s'arrêter et notifier dès qu'un conflit est gros ou ambigu
trigger: always_on
---

# RULE : Résoudre seul les conflits de merge mécaniques, NOTIFIER dès qu'un conflit est gros ou ambigu

## Règle à appliquer

À chaque `git merge` / `git rebase` / `git pull` qui produit des conflits, **classer chaque conflit avant d'y toucher**, puis appliquer la règle de sa catégorie.

### Conflits « mécaniques » — à résoudre seul

La bonne résolution est déductible sans arbitrage :

- un côté est un **sur-ensemble** strict de l'autre ;
- **renommage vs édition** du même symbole, où les deux intentions se composent ;
- fichiers d'**accumulation** (liste de sources dans un `CMakeLists.txt`, liste de dépendances, README) : la résolution est l'**union** des deux côtés ;
- conflit purement textuel sur un commentaire ou un doc-comment.

Après résolution : build et tests verts avant de committer le merge.

### Conflits « gros » — STOP, notifier, attendre

Interrompre dès qu'un seul signal est présent :

- les deux côtés ont modifié la **même logique** dans des directions différentes ;
- le conflit porte sur un **contrat** : signature publique, interface, format de paquet, identifiant de type de paquet, preset CMake, baseline vcpkg ;
- résoudre exigerait de **supprimer du code de l'autre côté** dont on ne connaît pas l'intention ;
- plus de ~10 hunks, ou un hunk de plus de ~40 lignes ;
- le moindre doute sur ce que l'autre côté cherchait à faire.

Dans ce cas : ne pas résoudre, laisser le merge en cours (ou `git merge --abort` si l'état est confus), et exposer pour chaque conflit litigieux le fichier, ce que fait chaque côté, et l'option recommandée.

### Toujours

- **Jamais `git checkout --ours/--theirs` sur un fichier entier** : ça jette aussi les hunks auto-mergés du fichier. On édite les marqueurs.
- **Jamais de commit contenant encore `<<<<<<<`** : `git diff --check` et `grep -rn '^<<<<<<<'` avant de committer.
- **Annoncer le bilan** : combien de conflits, dans quels fichiers, résolus comment.

## Exemple

- ❌ **Interdit** : `git checkout --ours src/server/CMakeLists.txt` pour purger les marqueurs → les sources ajoutées par l'autre branche disparaissent du build.
- ✅ **À la place** : éditer le hunk en gardant l'union des deux listes de sources, builder, committer, et dire ce qui a été résolu.
