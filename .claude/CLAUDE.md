# CLAUDE.md — R-Type

Instructions pour les agents qui travaillent sur ce dépôt. Elles complètent le `CLAUDE.md` global de chaque développeur, elles ne l'assouplissent jamais.

## Le projet

Remake multijoueur en réseau du shoot'em up R-Type, sur un moteur de jeu C++ maison. Projet Epitech tek3, en équipe.

| Élément | Choix |
|---|---|
| Langage | C++20, compilé par GCC/Clang (Linux) et MSVC (Windows) |
| Build | CMake ≥ 3.28, presets (`default`, `test`), dépendances par vcpkg (sous-module épinglé) |
| Bibliothèques | SFML 3 (`graphics`, `window`, `audio`) côté client, Asio (standalone) côté serveur, GoogleTest pour les tests |
| Cibles | `r-type_server` (Asio uniquement, jamais SFML), `r-type_client` (SFML), `harness_tests` |
| Style | `.clang-format` (base Google, indentation 2, 80 colonnes), `.clang-tidy` (warnings = erreurs) |

L'arborescence `src/server`, `src/client`, `tests` est provisoire : l'organisation moteur/jeu n'est pas encore décidée. Lire le dépôt avant de supposer une structure.

## Commandes

```bash
git submodule update --init
cmake --workflow --preset build                 # Release, binaires à la racine
cmake --workflow --preset test                  # Debug + tests, DOIT être vert, zéro warning
cmake --build build --target format-check       # formatage
cmake --build build --target format             # formate en place
cmake --build build --target tidy               # clang-tidy
```

La CI (`build-and-test`) vérifie le formatage puis build et teste sur Linux (GCC) et Windows (MSVC), warnings traités en erreurs.

## Rules

Chaque fichier de `.claude/rules/` (sous-dossiers compris) est une règle isolée, chargée automatiquement. Les appliquer n'est pas optionnel : une règle violée est un défaut, pas une préférence. En cas d'erreur, `core/errors-learning.md` impose d'en créer une nouvelle.

Les chemins ci-dessous sont relatifs à `.claude/rules/`.

- **Sans `paths`** (`core/`, `fix/`) : chargées à chaque session.
- **Avec `paths: ["**/*.{cpp,hpp,tpp}"]`** (`architecture/`, `code-style/`) : chargées seulement quand un fichier C++ est lu. **Avant d'écrire du C++ sans avoir lu de fichier C++ dans la session** (nouveau fichier, nouveau module), lire ces deux dossiers explicitement.

### Points d'entrée par situation

| Situation | À appliquer |
|---|---|
| Écrire du C++ | `architecture/`, `code-style/` |
| Écrire un test | `code-style/code-style-tests-given-when-then.md`, `fix/process/fix-process-test-vert-qui-verrouille-un-defaut-et-fake-trop-deterministe.md` |
| Toucher au réseau | section sécurité de `core/code-review.md` |
| Créer une branche | `fix/process/fix-process-brancher-avant-le-premier-commit-jamais-sur-la-branche-en-cours.md`, `fix/execution/fix-execution-checkout-b-part-de-head-pas-de-main.md` |
| Committer | `core/commit.md`, `fix/process/fix-process-pas-de-co-author-commit.md`, `fix/execution/fix-execution-verifier-l-index-avant-de-committer.md` |
| Ouvrir une PR | `fix/format/fix-format-pull-requests-en-anglais.md`, `fix/process/fix-process-pr-toujours-creee-en-draft.md` |
| Faire une revue | `core/code-review.md` |
| Annoncer un gate vert | `fix/execution/fix-execution-zsh-pipestatus.md`, `fix/execution/fix-execution-zero-warning-se-prouve-sur-un-build-qui-recompile.md` |
| Se tromper | `core/errors-learning.md` — l'erreur produit une nouvelle règle |
| Faire une pause, finir une étape | `core/context-continuity.md` |

### Index

**Contrat — `core/`** (chargées à chaque session)
- `core/code-review.md`
- `core/commit.md`
- `core/context-continuity.md`
- `core/errors-learning.md`

**Architecture C++ — `architecture/`** (chargées à la lecture d'un fichier C++)
- `architecture/architecture-exceptions-custom-par-module.md`
- `architecture/architecture-hpp-declarations-cpp-definitions.md`
- `architecture/architecture-interfaces-purement-virtuelles.md`
- `architecture/fix-architecture-urls-et-valeurs-magiques-en-constantes.md`

**Style de code — `code-style/`** (chargées à la lecture d'un fichier C++)
- `code-style/code-style-commentaires-anglais-hors-du-corps-sans-tickets.md`
- `code-style/code-style-constantes-sans-prefixe-k.md`
- `code-style/code-style-fichier-nomme-comme-sa-classe.md`
- `code-style/code-style-membres-prives-underscore-suffixe.md`
- `code-style/code-style-noms-complets-sans-abreviation.md`
- `code-style/code-style-tests-given-when-then.md`
- `code-style/fix-format-naming-donnees-pas-processus.md`

**Process — `fix/process/`**
- `fix/process/fix-process-brancher-avant-le-premier-commit-jamais-sur-la-branche-en-cours.md`
- `fix/process/fix-process-defaut-visible-jamais-classe-mineur-ni-descope-seul.md`
- `fix/process/fix-process-demander-l-observable-avant-de-batir-un-protocole.md`
- `fix/process/fix-process-instrumenter-avant-de-raisonner-la-doc-fait-foi.md`
- `fix/process/fix-process-ne-pas-sur-ingenier-prendre-du-recul.md`
- `fix/process/fix-process-pas-de-co-author-commit.md`
- `fix/process/fix-process-pr-toujours-creee-en-draft.md`
- `fix/process/fix-process-relayer-un-rapport-d-agent-sans-distinguer-le-verifie.md`
- `fix/process/fix-process-resolution-de-conflits-autonome-sauf-si-gros.md`
- `fix/process/fix-process-rework-incoherences-preexistantes.md`
- `fix/process/fix-process-test-vert-qui-verrouille-un-defaut-et-fake-trop-deterministe.md`

**Raisonnement — `fix/raisonnement/`**
- `fix/raisonnement/fix-raisonnement-code-present-nest-pas-comportement.md`
- `fix/raisonnement/fix-raisonnement-contrat-borne-verifier-l-etat-vide.md`
- `fix/raisonnement/fix-raisonnement-convention-derivee-d-un-seul-echantillon.md`
- `fix/raisonnement/fix-raisonnement-garde-fou-apres-l-ecriture-n-est-qu-une-alarme.md`
- `fix/raisonnement/fix-raisonnement-invariant-delegue-doit-etre-calcule.md`
- `fix/raisonnement/fix-raisonnement-ne-pas-halluciner-contenu-fichier.md`
- `fix/raisonnement/fix-raisonnement-neutre-en-prod-et-benefique-en-test-nommer-l-axe.md`
- `fix/raisonnement/fix-raisonnement-preuve-qui-ne-discrimine-pas.md`
- `fix/raisonnement/fix-raisonnement-un-type-derreur-nest-pas-un-diagnostic.md`
- `fix/raisonnement/fix-raisonnement-une-sequence-asserte-son-etat-de-depart.md`
- `fix/raisonnement/fix-raisonnement-utiliser-la-mesure-pour-falsifier-avant-de-coder.md`
- `fix/raisonnement/fix-raisonnement-valider-l-instrument-avant-de-rapporter-un-compte.md`
- `fix/raisonnement/fix-raisonnement-verifier-l-artefact-livre-pas-son-empreinte.md`
- `fix/raisonnement/fix-raisonnement-verifier-l-etat-du-code-avant-de-scoper.md`
- `fix/raisonnement/fix-raisonnement-verifier-la-premisse-avant-de-corriger-l-aval.md`

**Exécution (shell, git, build) — `fix/execution/`**
- `fix/execution/fix-execution-checkout-b-part-de-head-pas-de-main.md`
- `fix/execution/fix-execution-git-stash-pop-sans-push-effectif.md`
- `fix/execution/fix-execution-git-verifier-la-branche-avant-ecriture-d-historique.md`
- `fix/execution/fix-execution-heredoc-non-quote-le-shell-execute-les-backticks.md`
- `fix/execution/fix-execution-nouvel-appelant-reprendre-la-garde-des-appelants-existants.md`
- `fix/execution/fix-execution-push-silencieux-et-head-detache-verifier-le-sha-distant.md`
- `fix/execution/fix-execution-script-d-edition-valide-ses-ancres-avant-d-ecrire.md`
- `fix/execution/fix-execution-touch-recree-les-fichiers-supprimes.md`
- `fix/execution/fix-execution-valider-contre-la-version-de-la-cible.md`
- `fix/execution/fix-execution-verifier-l-index-avant-de-committer.md`
- `fix/execution/fix-execution-zero-warning-se-prouve-sur-un-build-qui-recompile.md`
- `fix/execution/fix-execution-zsh-caracteres-speciaux-non-quotes.md`
- `fix/execution/fix-execution-zsh-pipestatus.md`
- `fix/execution/fix-execution-zsh-variable-colon-modifiers.md`
- `fix/execution/fix-execution-zsh-variable-path-minuscule-ecrase-le-PATH.md`
- `fix/execution/fix-execution-zsh-word-splitting.md`

**Format — `fix/format/`**
- `fix/format/fix-format-poser-le-contexte-avant-le-livrable.md`
- `fix/format/fix-format-pull-requests-en-anglais.md`
- `fix/format/fix-format-write-tool-pas-d-artefact-tool-call.md`

**Interprétation — `fix/interpretation/`**
- `fix/interpretation/fix-interpretation-bouche-trou-signale-a-remplacer.md`
- `fix/interpretation/fix-interpretation-guideline-positive-nest-pas-une-interdiction.md`
- `fix/interpretation/fix-interpretation-negation-ambigue-confirmer-avant-d-inverser.md`
- `fix/interpretation/fix-interpretation-typo-lecture-qui-contredit-une-regle.md`

## Definition of done

- [ ] Branche dédiée partie d'`origin/main`, une intention par commit.
- [ ] `cmake --workflow --preset test` vert, **zéro warning**, sortie et code de retour lus.
- [ ] `format-check` et `tidy` verts.
- [ ] Aucune violation des rules de `architecture/` et `code-style/`.
- [ ] Tests Given / When / Then pour le comportement ajouté ou corrigé.
- [ ] Commits et PR en anglais, **aucune mention d'IA**, PR ouverte en draft.
