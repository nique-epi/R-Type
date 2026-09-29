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

## Rules — toujours actives

Chaque fichier de `.claude/rules/` est une règle isolée, chargée automatiquement à chaque session. Les appliquer n'est pas optionnel : une règle violée est un défaut, pas une préférence. En cas d'erreur, `errors-learning.md` impose d'en créer une nouvelle.

### Points d'entrée par situation

| Situation | À appliquer |
|---|---|
| Écrire du C++ | `architecture-*.md`, `code-style-*.md`, `fix-architecture-*.md` |
| Écrire un test | `code-style-tests-given-when-then.md`, `fix-process-test-vert-qui-verrouille-un-defaut-et-fake-trop-deterministe.md` |
| Toucher au réseau | section sécurité de `code-review.md` |
| Créer une branche | `fix-process-brancher-avant-le-premier-commit-jamais-sur-la-branche-en-cours.md`, `fix-execution-checkout-b-part-de-head-pas-de-main.md` |
| Committer | `commit.md`, `fix-process-pas-de-co-author-commit.md`, `fix-execution-verifier-l-index-avant-de-committer.md` |
| Ouvrir une PR | `fix-format-pull-requests-en-anglais.md`, `fix-process-pr-toujours-creee-en-draft.md` |
| Faire une revue | `code-review.md` |
| Annoncer un gate vert | `fix-execution-zsh-pipestatus.md`, `fix-execution-zero-warning-se-prouve-sur-un-build-qui-recompile.md` |
| Se tromper | `errors-learning.md` — l'erreur produit une nouvelle règle |
| Faire une pause, finir une étape | `context-continuity.md` |

### Index

**Contrat**
- `errors-learning.md` — chaque erreur produit une rule `fix-*`
- `context-continuity.md` — point de reprise dans `.context/context.md`
- `commit.md` — Conventional Commits en anglais
- `code-review.md` — procédure, sévérités, critères, format du rapport

**Architecture C++**
- `architecture-hpp-declarations-cpp-definitions.md`
- `architecture-interfaces-purement-virtuelles.md`
- `architecture-exceptions-custom-par-module.md`
- `fix-architecture-urls-et-valeurs-magiques-en-constantes.md`

**Style de code**
- `code-style-constantes-sans-prefixe-k.md`
- `code-style-noms-complets-sans-abreviation.md`
- `code-style-membres-prives-underscore-suffixe.md`
- `code-style-fichier-nomme-comme-sa-classe.md`
- `code-style-commentaires-anglais-hors-du-corps-sans-tickets.md`
- `code-style-tests-given-when-then.md`
- `fix-format-naming-donnees-pas-processus.md`

**Process**
- `fix-process-pas-de-co-author-commit.md`
- `fix-process-pr-toujours-creee-en-draft.md`
- `fix-process-brancher-avant-le-premier-commit-jamais-sur-la-branche-en-cours.md`
- `fix-process-rework-incoherences-preexistantes.md`
- `fix-process-defaut-visible-jamais-classe-mineur-ni-descope-seul.md`
- `fix-process-ne-pas-sur-ingenier-prendre-du-recul.md`
- `fix-process-instrumenter-avant-de-raisonner-la-doc-fait-foi.md`
- `fix-process-demander-l-observable-avant-de-batir-un-protocole.md`
- `fix-process-test-vert-qui-verrouille-un-defaut-et-fake-trop-deterministe.md`
- `fix-process-relayer-un-rapport-d-agent-sans-distinguer-le-verifie.md`
- `fix-process-resolution-de-conflits-autonome-sauf-si-gros.md`

**Raisonnement**
- `fix-raisonnement-ne-pas-halluciner-contenu-fichier.md`
- `fix-raisonnement-convention-derivee-d-un-seul-echantillon.md`
- `fix-raisonnement-invariant-delegue-doit-etre-calcule.md`
- `fix-raisonnement-neutre-en-prod-et-benefique-en-test-nommer-l-axe.md`
- `fix-raisonnement-code-present-nest-pas-comportement.md`
- `fix-raisonnement-verifier-la-premisse-avant-de-corriger-l-aval.md`
- `fix-raisonnement-verifier-l-etat-du-code-avant-de-scoper.md`
- `fix-raisonnement-un-type-derreur-nest-pas-un-diagnostic.md`
- `fix-raisonnement-utiliser-la-mesure-pour-falsifier-avant-de-coder.md`
- `fix-raisonnement-une-sequence-asserte-son-etat-de-depart.md`
- `fix-raisonnement-preuve-qui-ne-discrimine-pas.md`
- `fix-raisonnement-valider-l-instrument-avant-de-rapporter-un-compte.md`
- `fix-raisonnement-verifier-l-artefact-livre-pas-son-empreinte.md`
- `fix-raisonnement-contrat-borne-verifier-l-etat-vide.md`
- `fix-raisonnement-garde-fou-apres-l-ecriture-n-est-qu-une-alarme.md`

**Exécution (shell, git, build)**
- `fix-execution-zsh-pipestatus.md`
- `fix-execution-zsh-word-splitting.md`
- `fix-execution-zsh-variable-colon-modifiers.md`
- `fix-execution-zsh-variable-path-minuscule-ecrase-le-PATH.md`
- `fix-execution-zsh-caracteres-speciaux-non-quotes.md`
- `fix-execution-heredoc-non-quote-le-shell-execute-les-backticks.md`
- `fix-execution-git-verifier-la-branche-avant-ecriture-d-historique.md`
- `fix-execution-git-stash-pop-sans-push-effectif.md`
- `fix-execution-checkout-b-part-de-head-pas-de-main.md`
- `fix-execution-verifier-l-index-avant-de-committer.md`
- `fix-execution-push-silencieux-et-head-detache-verifier-le-sha-distant.md`
- `fix-execution-script-d-edition-valide-ses-ancres-avant-d-ecrire.md`
- `fix-execution-touch-recree-les-fichiers-supprimes.md`
- `fix-execution-zero-warning-se-prouve-sur-un-build-qui-recompile.md`
- `fix-execution-valider-contre-la-version-de-la-cible.md`
- `fix-execution-nouvel-appelant-reprendre-la-garde-des-appelants-existants.md`

**Format et interprétation**
- `fix-format-pull-requests-en-anglais.md`
- `fix-format-poser-le-contexte-avant-le-livrable.md`
- `fix-format-write-tool-pas-d-artefact-tool-call.md`
- `fix-interpretation-negation-ambigue-confirmer-avant-d-inverser.md`
- `fix-interpretation-typo-lecture-qui-contredit-une-regle.md`
- `fix-interpretation-bouche-trou-signale-a-remplacer.md`
- `fix-interpretation-guideline-positive-nest-pas-une-interdiction.md`

## Definition of done

- [ ] Branche dédiée partie d'`origin/main`, une intention par commit.
- [ ] `cmake --workflow --preset test` vert, **zéro warning**, sortie et code de retour lus.
- [ ] `format-check` et `tidy` verts.
- [ ] Aucune violation des rules `architecture-*` et `code-style-*`.
- [ ] Tests Given / When / Then pour le comportement ajouté ou corrigé.
- [ ] Commits et PR en anglais, **aucune mention d'IA**, PR ouverte en draft.
