---
description: Le contenu écrit dans un fichier ne contient jamais de fragment de syntaxe d'appel d'outil ; un gros fichier se relit en fin d'écriture
trigger: always_on
---

# RULE : Ne jamais laisser un artefact d'appel d'outil dans le CONTENU écrit d'un fichier

## Règle à appliquer

1. **Le contenu passé à l'outil d'écriture ne contient jamais de fragment de la syntaxe d'appel d'outil** (balise de fermeture d'invocation, balise de paramètre, JSON d'appel tronqué). Ce sont des artefacts de la mécanique d'outil, jamais du contenu.
2. **Le contenu est complet et cohérent du premier au dernier caractère** : chaque bloc de code ouvert est fermé, chaque balise ouverte a sa fermeture.
3. **Pour un fichier long**, écrire une base complète et valide, puis l'étendre par des éditions ciblées — jamais une écriture partielle qui laisse un bloc ouvert.
4. **Après l'écriture d'un gros contenu, contrôler la fin du fichier** (dernières lignes, `grep` d'un nom de balise d'invocation) avant de passer à la suite.

## Exemple

- ❌ **Avant (incorrect)** : un `.md` de rule qui s'arrête au milieu d'un bloc ```` ```cpp ```` jamais fermé, suivi d'un fragment de balise d'appel d'outil.
- ✅ **Après (correct)** : contenu complet, blocs fermés, puis `tail -5` du fichier pour le vérifier.
