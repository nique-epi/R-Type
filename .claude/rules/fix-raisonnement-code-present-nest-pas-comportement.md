---
description: Du code présent n'est pas un comportement prouvé — exiger une preuve d'exécution avant de dire « ça existe déjà »
trigger: always_on
---

# RULE : Du code présent n'est pas un comportement — exiger une preuve d'exécution

## Règle à appliquer

1. **Avant de répondre « ça existe déjà »** sur une fonctionnalité visible ou comportementale (un son, une animation, un paquet envoyé, un ennemi qui apparaît), exiger une **preuve d'exécution** — un log, un test qui la traverse, une capture, le témoignage de l'utilisateur — pas une preuve de code. À défaut, dire « le code est là, je n'ai pas de preuve qu'il produit l'effet », et vérifier.
2. **Une déclaration n'est pas un appel.** Un type de paquet déclaré dans l'`enum`, un système enregistré mais jamais ajouté à la boucle, une méthode sans site d'appel : chercher le **site d'appel** (`grep -rn`) avant d'affirmer qu'une chose se produit.
3. **Tout comportement d'API supposé se vérifie dans la doc ou les headers** avant d'être affirmé ; un doc-comment maison ne compte pas comme source (cf. `fix-process-instrumenter-avant-de-raisonner-la-doc-fait-foi.md`).
4. **Quand un diagnostic tourne en rond** (deux relectures qui concluent « c'est correct » face à un « ça ne marche pas »), arrêter de relire le câblage et vérifier que la **brique terminale** produit son effet.

## Exemple

- ❌ **Avant (incorrect)** : « le son de tir existe déjà, `SoundSystem` gère `ShootEvent` » → le son ne joue jamais : `SoundSystem` n'est pas dans la liste des systèmes exécutés.
- ✅ **Après (correct)** : `grep -rn "SoundSystem" src` → déclaré, jamais ajouté à la boucle → le dire, puis corriger l'enregistrement.
