---
description: Auto-correction — créer une rule fix-* corrective à chaque erreur pour empêcher sa répétition
trigger: always_on
---

# RULE : Auto-correction par création de rules

## Objectif

Chaque fois qu'une erreur est commise — de raisonnement, d'exécution, d'interprétation, de format ou de comportement — une **rule corrective dédiée doit être créée** pour empêcher sa répétition. On construit ainsi, en continu, une base de connaissances corrective propre à **ce projet**. Cette règle est **toujours active** : la création d'une rule corrective fait partie intégrante du traitement de l'erreur, jamais reportée.

## Déclencheurs

Créer une rule corrective quand :

1. **Erreur de raisonnement** — déduction incorrecte, confusion de deux concepts, logique erronée.
2. **Erreur d'exécution** — script/commande qui échoue à cause d'une syntaxe, d'un chemin ou d'un paramètre mal généré.
3. **Erreur d'interprétation** — mauvaise compréhension de la demande, hypothèse fausse sur le contexte ou l'état du code.
4. **Erreur de format / output** — livrable dans le mauvais format, mauvaise structure, conventions non respectées.
5. **Erreur de process** — étape oubliée, mauvais ordre, contrainte explicite ignorée.
6. **Régression** — répétition d'une erreur déjà signalée ou corrigée.
7. **Correction par l'utilisateur** — l'utilisateur corrige explicitement, même sur un point mineur.

## Procédure

### Étape 1 — Identifier et reconnaître

Nommer l'erreur clairement, expliquer brièvement ce qui s'est passé, ne pas la noyer dans des excuses.

### Étape 2 — Créer la rule corrective, en deux fichiers de même nom

Le cœur est chargé à chaque session ; l'historique ne l'est pas. Seul ce qui prescrit va dans le cœur, le récit de l'incident va dans l'historique.

**Cœur** — `.claude/rules/fix-[catégorie]-[description-courte].md` :

```markdown
---
description: <une ligne>
trigger: always_on
---

# RULE : [Titre court et descriptif]

Pourquoi : [une phrase, seulement si la règle ne se comprend pas sans elle]

## Règle à appliquer
[Instruction claire, impérative, actionnable, suivable sans contexte supplémentaire]

## Exemple
- ❌ **Avant (incorrect)** : [ce qui a été fait]
- ✅ **Après (correct)** : [ce qu'il faut faire à la place]
```

**Historique** — `.claude/rules-history/fix-[catégorie]-[description-courte].md`, jamais chargé (hors de `.claude/rules/`, que Claude Code charge en entier) : le fichier complet, avec `## Contexte`, `## Erreur commise`, `## Cause racine`, puis la règle et l'exemple.

**Récidive ou complément** : la prescription s'ajoute au cœur sous un sous-titre `### <titre court>` de « Règle à appliquer » ; le récit s'ajoute à l'historique dans une section `## Mise à jour (date) — <titre>`.

**Quand lire l'historique** — il porte le même nom que la rule, dans `.claude/rules-history/`, et se lit à la demande :
- avant d'enrichir une rule sur une récidive (savoir ce qui a déjà été essayé et raté) ;
- quand la portée d'une rule est douteuse sur le cas en cours (l'incident d'origine dit ce qu'elle visait) ;
- quand l'utilisateur demande pourquoi une rule existe.

Les rules importées d'autres projets au démarrage de R-Type n'ont pas d'historique ici : leur incident d'origine appartient à un autre dépôt.

### Étape 3 — Confirmer

Indiquer le nom du fichier créé, résumer la règle en une phrase, reprendre la tâche en appliquant immédiatement la correction.

## Règles sur les rules

- **Granularité** : une rule = une erreur spécifique. Pas de rule fourre-tout.
- **Clarté** : la section « Règle à appliquer » est une instruction simple, autoportante.
- **Pas de doublons** : avant de créer, vérifier qu'aucune rule ne couvre déjà le cas. Si oui, l'enrichir plutôt que d'en créer une nouvelle.
- **Nommage cohérent** : catégories `raisonnement`, `execution`, `interpretation`, `format`, `process`, `architecture`.
- **Langue** : français (langue de travail du projet). Les exemples de code restent en anglais, comme le code.
- **Dépôt public** : une rule ou un historique ne contient ni secret, ni donnée personnelle, ni détail d'un autre projet.
- **Portée** : les `fix-*.md` sont contraignants au même titre que les rules de contrat. Les consulter et les appliquer avant de coder ou de livrer.
- **Index** : toute nouvelle rule s'ajoute à la table de `.claude/CLAUDE.md`.

## Exemples

- ❌ **Interdit** : détecter une erreur, s'excuser, et reprendre sans rien acter — l'erreur pourra se reproduire.
- ✅ **À la place** : détecter l'erreur → créer `fix-[catégorie]-[slug].md` (+ son historique) → confirmer → reprendre en appliquant la correction.
