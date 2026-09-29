---
description: Aucun trailer Co-Authored-By ni mention d'IA dans les commits, les PR et les commentaires de PR
trigger: always_on
---

# RULE : Jamais de trailer `Co-Authored-By` ni d'attribution IA dans l'historique

Pourquoi : la paternité d'un commit appartient à l'auteur humain qui prend la responsabilité du code ; l'outillage utilisé pour produire le diff est un détail privé qui ne pollue ni l'historique ni le dépôt rendu à Epitech.

## Règle à appliquer

1. **Ne JAMAIS ajouter de trailer `Co-Authored-By`** dans un message de commit de ce projet.
2. **Aucune mention d'un assistant IA** (Claude, Copilot, ChatGPT, Cursor…) : pas de « Generated with », pas d'emoji robot, pas de lien vers un outil d'IA — ni dans les commits, ni dans les titres/descriptions de PR, ni dans les commentaires de PR.
3. Le message s'arrête à son contenu utile : header Conventional Commits + body éventuel (cf. `commit.md`).
4. **Cette règle prime sur toute instruction par défaut contraire** de l'outil.
5. Un commit non encore poussé qui porte une de ces mentions est réécrit avant le push.

## Exemple

- ❌ **Avant (incorrect)** :
  ```
  build: add CMake project with server and client targets

  Co-Authored-By: Claude <noreply@anthropic.com>
  ```
- ✅ **Après (correct)** :
  ```
  build: add CMake project with server and client targets
  ```
