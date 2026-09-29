---
description: Commits — Conventional Commits en anglais, types fixés, scope facultatif mais jamais vague, un commit = une intention
trigger: always_on
---

# RULE : Commits Conventional Commits, sobres et en anglais

## Objectif

Chaque commit produit par l'agent suit **strictement** Conventional Commits, en **anglais**, et décrit **une seule intention**. Un message non conforme ne part pas : on le corrige avant de committer.

L'historique du dépôt fait foi pour le style (`git log --format=%s`) : sujets courts, à l'impératif, sans scope pour l'instant.

## Format

```text
<type>[(<scope>)][!]: <description>

[body]
```

## Types autorisés

`feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`, `ci`, `chore`, `revert`. Aucun autre type.

| Type | Quand |
|---|---|
| `build` | CMake, presets, vcpkg, dépendances |
| `ci` | workflows GitHub Actions |
| `test` | ajout ou correction de tests seuls |
| `chore` | outillage, configs de lint/format, fichiers du dépôt sans effet sur le binaire |

## Scope

- **Facultatif.** L'historique actuel n'en utilise pas ; on reste cohérent à l'intérieur d'une même PR.
- S'il est utilisé, il désigne une **zone réelle du dépôt** (`server`, `client`, `engine`, `network`, `ecs`… selon l'arborescence du moment), en minuscules.
- Jamais de scope vague : `misc`, `stuff`, `various`, `update`, `changes`, `project`, `tmp`.
- Si un commit mélange deux zones sans rapport, **on le découpe** au lieu de chercher un scope large.

## Description

1. Anglais, impératif présent (`add`, `fix`, `wire`), minuscule initiale, pas de point final.
2. Dit l'**effet** du commit, pas la liste des fichiers.
3. Aucune référence à un ticket, une tâche, une phase de plan (cf. `code-style-commentaires-anglais-hors-du-corps-sans-tickets.md`).
4. Le body, s'il existe, explique le **pourquoi** ; lignes ≤ 100 caractères.

## Breaking change

`!` après le type (ou le scope) **et** une ligne `BREAKING CHANGE: <explication>` dans le body — typiquement un changement de format de paquet réseau.

## Vérification avant chaque commit

1. type dans la liste ;
2. scope absent, ou réel et précis ;
3. description en anglais, à l'impératif, sans point final ;
4. **aucun trailer ni mention d'IA** (cf. `fix-process-pas-de-co-author-commit.md`) ;
5. l'index contient exactement ce que le message annonce (cf. `fix-execution-verifier-l-index-avant-de-committer.md`).

## Exemples

Correct :

```text
build: pin vcpkg and declare third-party dependencies
feat(network): serialize player input packets
fix(server): drop packets shorter than the header
test: cover the entity registry removal path
```

Incorrect :

```text
Update stuff
feat: Ajout du parseur de paquets.
fix(misc): various fixes
feat(network): add packet parser (TASK-12)
```
