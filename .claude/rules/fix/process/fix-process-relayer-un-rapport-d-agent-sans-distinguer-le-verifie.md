---
description: Un rapport de sous-agent se relaie en distinguant ce qu'il a vérifié lui-même de ce qu'il a délégué ou supposé
trigger: always_on
---

# RULE : Ne jamais relayer les faits d'un rapport d'agent sans distinguer ce qu'il a VÉRIFIÉ de ce qu'il a délégué ou supposé

Pourquoi : un rapport dense, structuré, plein de citations et de références imite la forme d'un travail sourcé ; la densité ne coûte rien à produire sans les sources.

## Règle à appliquer

1. **Exiger la distinction dans le brief.** Tout brief de recherche ou d'audit délégué demande, pour chaque affirmation : *vérifiée en propre* (avec le fichier lu, la commande ou l'URL réellement appelée), *rapportée par un sous-agent*, ou *non vérifiée*. Un rapport sans cette distinction est incomplet, quelle que soit sa qualité apparente.
2. **Interdire d'écrire une section dont la source n'a pas répondu.** Un sous-agent qui ne rend rien produit une section « non couverte », jamais une section rédigée de mémoire.
3. **Ne jamais relayer un chiffre, une citation ou un comportement d'API sans savoir qui l'a lu.** Les affirmations porteuses de décision sont soit vérifiées par moi, soit attribuées explicitement (« l'agent rapporte, non vérifié »).
4. **Redoubler de vigilance quand le rapport sert à me corriger.** Une source qui renverse ma position mérite une vérification plus stricte, pas moins.
5. **Une délégation en cascade est un point de défaillance de plus** : le brief impose que le résultat d'un sous-agent soit cité tel quel, avec la mention de son absence si elle survient.

## Exemple

- ❌ **Avant (incorrect)** : l'agent écrit « Asio garantit que les handlers d'un même `strand` ne s'exécutent jamais en parallèle, et SFML 3 est thread-safe pour le rendu » ; je relaie les deux comme établis.
- ✅ **Après (correct)** : brief exigeant le marquage vérifié/délégué/non vérifié ; au relais, la garantie des `strand` (lue dans la doc Asio, lien cité) est présentée comme établie, l'affirmation sur SFML comme non vérifiée — et je la vérifie avant d'en faire une décision.
