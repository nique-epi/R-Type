---
description: Prouver la prémisse (l'état demandé est-il l'état appliqué ?) avant de corriger un comportement qui en dérive
trigger: always_on
---

# RULE : Vérifier la prémisse avant de corriger l'aval

## Règle à appliquer

1. **Avant de corriger un comportement dérivé d'un état, prouver l'état.** Le premier log à poser affiche côte à côte **ce qui est demandé** et **ce qui est effectivement appliqué** (position envoyée vs position rendue, tick demandé vs tick simulé). Un désaccord entre les deux clôt le débat.
2. **Relire ses propres logs comme un adversaire.** Une ligne écrite pour confirmer une hypothèse contient souvent la réfutation d'une autre : à chaque champ affiché, se demander ce qu'il dirait si l'hypothèse était fausse.
3. **Un symptôme qui pointe systématiquement le premier élément** (première entité, premier joueur, index 0, id 0) est presque toujours un **défaut d'origine** : quelque chose n'a jamais quitté sa valeur initiale.
4. **Quand deux leviers visent la même cible sans pouvoir se contredire**, un correctif juste sous les deux hypothèses est préférable à un correctif juste sous une seule.

## Exemple

- ❌ **Avant (incorrect)** : « les tirs partent du mauvais vaisseau » → corriger le calcul d'offset du canon, puis la rotation, puis l'interpolation — trois rounds.
- ✅ **Après (correct)** : log `requested owner=3, spawned owner=0` → le propriétaire n'est jamais renseigné à la création du projectile → corriger l'origine, puis vérifier s'il reste quelque chose à corriger en aval.
