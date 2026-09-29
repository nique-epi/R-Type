---
description: Revue de code — procédure de lancement, sévérités, critères C++/réseau, règles anti faux-positifs, format du rapport
trigger: always_on
---

# RULE : Revue de code — critères, sévérités et procédure de lancement

## Objectif

Quand l'utilisateur demande une **review** (« lance une review », « review cette PR », « fais reviewer ça »), l'agent applique **cette rule intégralement**. Elle définit ce qu'on cherche, comment on le classe, ce qu'on a le droit de rapporter, et comment la review est déléguée.

Une review qui ne suit pas cette rule n'est pas une review : c'est une lecture.

## 1. Procédure de lancement

1. **Déléguer à un sous-agent**, jamais faire la review soi-même dans le contexte principal : la review doit être faite par un lecteur qui n'a pas écrit le code et n'a pas le biais de l'intention.
2. **Le brief de l'agent contient**, explicitement :
   - le **diff exact** (`git diff origin/main...HEAD` ou la liste de fichiers) ;
   - le **contexte fonctionnel** (ce que la PR est censée faire, et pourquoi) ;
   - le **chemin de cette rule** + les `.claude/rules/` pertinentes au diff ;
   - les **gates déjà passés** (build, tests, `format-check`, `tidy`) avec leur sortie réelle ;
   - l'ordre de **vérifier chaque finding dans le code avant de le rapporter**.
3. **Ne jamais donner à l'agent un invariant « à asserter » qu'on n'a pas mesuré soi-même** (cf. `fix-raisonnement-invariant-delegue-doit-etre-calcule.md`). On lui demande de dériver et mesurer, pas de confirmer une hypothèse.
4. **Plusieurs agents en parallèle** si le diff couvre des domaines disjoints (ex. un pour le protocole réseau, un pour l'ECS, un pour le build/CI). Un seul agent pour un diff cohérent.
5. Le rapport revient à l'utilisateur **avec les findings, pas avec un résumé du diff**, en distinguant ce que l'agent a vérifié de ce qu'il suppose (cf. `fix-process-relayer-un-rapport-d-agent-sans-distinguer-le-verifie.md`).

## 2. Sévérités

Tout finding porte **exactement une** sévérité :

| Sévérité | Définition | Exemples |
|---|---|---|
| **Critical** | Crash, comportement indéfini, corruption d'état, faille exploitable à distance | lecture hors borne sur un paquet reçu, use-after-free, data race, serveur qui tombe sur un paquet malformé |
| **Warning** | Bug, régression de perf, anti-pattern structurel, violation d'une `.claude/rules/` | off-by-one, allocation par frame dans la boucle de jeu, logique métier dans un `.hpp`, `throw std::runtime_error` brut |
| **Info** | Style, lisibilité, suggestion mineure | nommage perfectible, simplification possible |

**Règle de tri** : dans le doute entre deux niveaux, prendre **le plus bas**. Un `Critical` qui n'en est pas un détruit la confiance dans tous les autres.

## 3. Ce qu'on cherche — dimensions génériques

Dans cet ordre d'importance :

1. **Design** — le code est-il au bon endroit (moteur vs jeu, client vs serveur, bon système ECS) ? Les dépendances vont-elles dans le bon sens (le serveur ne dépend jamais de SFML) ?
2. **Fonctionnalité** — le code fait-il ce que l'auteur voulait ? Cas limites, concurrence, erreurs non gérées, comportement sur Linux **et** Windows.
3. **Complexité** — peut-on faire plus simple ? **Sur-ingénierie** = un finding : du code qui résout un problème spéculatif futur plutôt que le besoin actuel.
4. **Tests** — existent-ils, et **échoueraient-ils vraiment** si le code cassait ? Pièges à chercher (cf. `fix-process-test-vert-qui-verrouille-un-defaut-et-fake-trop-deterministe.md`) : assertion recopiée de la sortie observée, assertion qui reconnaît une forme au lieu d'une valeur, double de test plus sage que la vraie dépendance (réseau sans perte ni réordonnancement, horloge figée).
5. **Nommage** — les noms disent-ils ce que la chose est ? (cf. `code-style-noms-complets-sans-abreviation.md`, `fix-format-naming-donnees-pas-processus.md`)
6. **Commentaires** — conformes à `code-style-commentaires-anglais-hors-du-corps-sans-tickets.md`.
7. **Style / cohérence** — conforme aux rules `code-style-*` et aux conventions voisines.
8. **Documentation** — README et doc du dépôt mis à jour si le comportement, le build ou le protocole change.
9. **Chaque ligne** — le diff se lit intégralement. Une ligne non comprise se dit, elle ne se survole pas.
10. **Contexte** — lire autour du diff. Le changement améliore-t-il ou dégrade-t-il la santé globale du code ?
11. **Réutilisation** — le diff invente-t-il un helper, un type ou un composant dont l'équivalent existe déjà ? Un concept qui acquiert une deuxième forme est un finding.
12. **Coût par frame / par tick** — pour tout code de la boucle de jeu ou du traitement réseau : allocations, copies, recherches linéaires en fonction du nombre d'entités ou de clients.

## 4. Ce qu'on cherche — sécurité et robustesse (systématique)

Le serveur reçoit des données de clients **non fiables** sur le réseau.

- **Entrées réseau** : toute taille, tout index, tout identifiant lu dans un paquet est borné **avant** usage. Un paquet tronqué, trop long ou d'un type inconnu est rejeté sans crash (cf. `architecture-parseur-parse-ne-valide-pas.md` pour *où* vit cette validation).
- **Mémoire** : pas de lecture/écriture hors borne, pas de pointeur ou référence qui survit à son propriétaire, pas de `reinterpret_cast` sur un buffer réseau sans contrôle de taille et d'alignement.
- **Endianness et types** : format sur le fil explicite (taille fixe, ordre des octets défini), jamais `sizeof` d'une struct non packée envoyée telle quelle.
- **Concurrence** : toute donnée partagée entre le thread réseau et la boucle de jeu est protégée ou transférée par une file dédiée.
- **Déni de service** : un client ne peut pas faire allouer au serveur une mémoire proportionnelle à une valeur qu'il contrôle.
- **Secrets** : aucune clé, token ou adresse privée en dur.

## 5. Gates spécifiques à ce dépôt

**Violation d'une de ces règles = `Warning` minimum, `Critical` si l'impact est un crash ou une faille.**

| Gate | Source |
|---|---|
| Aucun corps de méthode dans un `.hpp` (hors templates, `constexpr`, `= default`/`= delete`) | `architecture-hpp-declarations-cpp-definitions.md` |
| Interfaces : uniquement des méthodes `= 0` + destructeur virtuel `= default` | `architecture-interfaces-purement-virtuelles.md` |
| Erreurs métier en exceptions custom par module, jamais `throw std::…` brut | `architecture-exceptions-custom-par-module.md` |
| Un parseur/désérialiseur ne valide pas ; la validation vit dans une couche distincte | `architecture-parseur-parse-ne-valide-pas.md` |
| Nommage : pas de préfixe `k`, pas d'abréviation, `membre_` en suffixe, fichier = classe | `code-style-*.md` |
| Commentaires en anglais, hors du corps des fonctions, sans référence de ticket | `code-style-commentaires-anglais-hors-du-corps-sans-tickets.md` |
| Valeurs magiques en constantes nommées | `fix-architecture-urls-et-valeurs-magiques-en-constantes.md` |
| Commits Conventional Commits, **aucun trailer `Co-Authored-By`** ni mention d'IA | `commit.md`, `fix-process-pas-de-co-author-commit.md` |
| Incohérence pré-existante touchée : traitée ou signalée, jamais étendue | `fix-process-rework-incoherences-preexistantes.md` |

### Gates de vérification

| Gate | Commande |
|---|---|
| Build et tests verts, zéro warning, sortie réelle à l'appui | `cmake --workflow --preset test` |
| Formatage | `cmake --build build --target format-check` |
| Lint (warnings = erreurs) | `cmake --build build --target tidy` |
| Linux (GCC) **et** Windows (MSVC) | CI `build-and-test` de la PR |

## 6. Règles de rapport (anti faux-positifs)

Ces règles priment sur l'exhaustivité. Un rapport bruyant ne se lit pas.

1. **Tout finding est vérifié dans le code avant d'être écrit.** On cite `fichier:ligne`. Un finding sans localisation exacte n'est pas rapporté.
2. **Tout finding porte un scénario de casse concret** : entrées / état → sortie fausse ou crash. Si on ne sait pas l'écrire, le finding n'est pas mûr — on le descend en `Info` ou on le supprime.
3. **Pas de finding sur du code non touché par le diff**, sauf si le diff l'aggrave ou en dépend directement (finding de contexte, annoncé comme tel).
4. **Pas de préférence personnelle déguisée en défaut.** Un goût est un `Info` préfixé `Nit:`.
5. **Ne jamais affirmer un résultat de gate qu'on n'a pas lancé.** « Les tests passent » exige la sortie réelle et le code de retour lu correctement (cf. `fix-execution-zsh-pipestatus.md`).
6. **Signaler explicitement ce qui n'a pas pu être vérifié** plutôt que de laisser croire à une couverture complète.

## 7. Format de sortie

```markdown
## Verdict

<GO | GO-with-changes | NO-GO> — <une phrase>

## Critical (n)

### <titre court>

- **Où** : `src/server/Network/PacketReader.cpp:42`
- **Défaut** : <une phrase>
- **Casse** : <entrées/état → conséquence>
- **Correctif** : <la piste, pas un pavé>

## Warning (n)

<même structure>

## Info (n)

<liste courte, une ligne chacun>

## Non vérifié

<ce qui n'a pas pu l'être, et pourquoi>
```

**Verdict** : `NO-GO` uniquement s'il existe au moins un `Critical`. `GO-with-changes` si des `Warning` doivent être traités avant merge.

## Exemple

- ❌ **Avant (incorrect)** : « J'ai relu la PR, ça me semble propre, quelques remarques de style » — pas de sévérité, pas de localisation, pas de scénario, review faite par celui qui a écrit le code.
- ✅ **Après (correct)** : sous-agent briefé avec le diff + cette rule + les gates réels → `Critical` sur `PacketReader.cpp:42` (« `payloadSize` est lu dans l'en-tête puis utilisé pour copier depuis le buffer sans comparaison avec le nombre d'octets reçus → un client qui annonce 65535 octets dans un datagramme de 12 fait lire le serveur hors borne »), verdict `NO-GO`.
