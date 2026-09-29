---
description: Continuité de contexte — tenir .context/context.md à jour au fil de l'eau comme point de reprise
trigger: always_on
---

# RULE : Continuité de contexte

## Objectif

Le point de reprise du travail doit être **à jour en permanence**, pas seulement en fin de conversation. La prochaine session — ou un reset de contexte en plein milieu — doit pouvoir reprendre **sans re-lire ni ré-explorer** les mêmes fichiers. Chaque re-découverte est un coût payé deux fois.

Sur ce projet, le point de reprise est **`.context/context.md`**. Il est local à chaque développeur (ignoré par git) : il décrit **ma** session, pas l'état de l'équipe.

## Règles

- **Cadence** : mettre à jour `.context/context.md` **au fil de l'eau** — après chaque commit atomique ou unité logique de travail, à chaque décision d'architecture, quand on découvre le fonctionnement d'un sous-système (moteur, ECS, protocole réseau, build), à l'ouverture/merge d'une PR, à la correction d'un bug, et quand l'utilisateur demande de faire une pause. Une continuité écrite seulement à la fin est perdue si la session est interrompue avant.
- **Contenu** :
  - **Où on en est** : branche courante, étape, statut ;
  - **Ce qui a été fait** : commits, fichiers touchés, décisions prises ;
  - **Prochaine étape** : la commande ou l'action exacte pour reprendre ;
  - **Fichiers clés** du travail en cours ;
  - **En attente** : ce qui est inachevé ou attend l'utilisateur ;
  - **Décisions importantes** qui engagent la suite.
- **Compaction** : au-delà de ~150 lignes, archiver les étapes terminées dans une section « History » (2-3 lignes chacune), garder l'étape courante détaillée, conserver tous les items en attente et les décisions engageantes.
- **Format** : scannable — tables pour les listes de fichiers, puces pour les décisions, blocs de code pour les commandes. Pas de longs paragraphes.
- **Ce qui concerne l'équipe ne vit pas ici** : une décision d'architecture partagée va dans la doc du dépôt ou dans la PR, pas seulement dans un fichier ignoré.

## Exemples

- ❌ **Interdit** : enchaîner cinq commits puis mettre `.context/context.md` à jour d'un coup à la fin (ou pas du tout) → une interruption au commit 3 perd tout le fil.
- ✅ **À la place** : après chaque étape signifiante, refléter l'avancement dans `.context/context.md` **avant** de passer à la suite.
