---
description: Instrumenter les deux bouts d'un canal avant de raisonner ; un comportement d'API ne s'affirme qu'après lecture de la doc ou des headers
trigger: always_on
---

# RULE : Instrumenter les DEUX bouts d'un canal avant de raisonner, et ne conclure que sur la doc — jamais sur une intuition d'API

## Règle à appliquer

### 1. Instrumenter les deux bouts d'un canal, avant la première ligne de correctif

Dès qu'une valeur traverse une frontière — client → réseau → serveur, serveur → snapshot → client, entrée clavier → composant → système — et que le comportement observé ne colle pas, **poser un log à l'émission et un log à la réception**, avec l'identité de qui émet et de qui reçoit (id de client, numéro de séquence, id d'entité, tick). Un envoi sans réception correspondante *est* la réponse.

### 2. Deux allers-retours sans preuve = on arrête de coder

Au **2ᵉ** retour de l'utilisateur sur le même symptôme, plus aucun correctif ne part tant qu'une mesure n'a pas été produite : on instrumente, on fait lancer, on lit. C'est le « quoi faire à la place » de `fix-process-ne-pas-sur-ingenier-prendre-du-recul.md`.

### 3. La source de vérité est la documentation, pas l'intuition

Aucun comportement d'API ne s'affirme, ne se code, ni ne s'écrit en doc-comment sans avoir été **lu**, dans cet ordre :

1. les **headers de la version installée** (`build/vcpkg_installed/<triplet>/include/` après un configure) — ce sont eux qui décrivent la version réellement compilée (SFML 3 diffère largement de SFML 2) ;
2. la **documentation officielle** de cette version ;
3. une **recherche web**, dès que le sujet est un comportement subtil (durée de vie d'un buffer passé à `async_receive_from`, thread d'exécution d'un handler, ordre des événements SFML).

Citer la source dans la réponse. Une affirmation sans source est une hypothèse et se dit comme telle.

### 4. Un doc-comment ne contient que du vérifié

Une supposition écrite en commentaire devient un fait pour toutes les sessions suivantes.

### 5. Préférer l'API qui rend le bug impossible

Quand deux primitives existent, choisir celle dont la **forme** exclut la classe de bug. Un lookup qui échoue rend un `std::optional` vide ou un itérateur `end()`, jamais un voisin plausible (index 0, entité par défaut).

## Exemple

- ❌ **Avant (incorrect)** : « le client ne voit pas les autres joueurs, ça doit être l'interpolation » → trois correctifs sur l'interpolation.
- ✅ **Après (correct)** : un log à l'envoi du snapshot (tick, nombre d'entités) et un à sa réception (tick, nombre d'entités) → le client reçoit 0 entité → le serveur sérialise avant d'avoir ajouté les joueurs → correctif ciblé.
