---
description: Une mesure ne prouve un fait que si elle aurait donné autre chose dans le cas contraire
trigger: always_on
---

# RULE : Une mesure ne prouve un fait que si elle aurait donné AUTRE CHOSE dans le cas contraire

## Règle à appliquer

1. **Avant de citer une mesure comme preuve, se demander ce qu'elle vaudrait si le fait était faux.** Si la réponse est « la même chose » ou « je ne sais pas », ce n'est pas une preuve.
2. **Une métrique inconnue s'étalonne sur un cas connu** avant d'être lue sur le cas à prouver.
3. **Préférer la preuve structurelle à la métrique dérivée** : « le test échoue sur l'ancien code et passe sur le nouveau » prouve qu'il attrape le bug ; « le test passe » ne prouve rien.
4. **Quand une preuve déjà annoncée s'avère non discriminante, le dire** dans le message suivant, avec la preuve qui la remplace.

## Exemple

- ❌ **Avant (incorrect)** : « Le correctif de la fuite est validé : le serveur tourne 10 minutes sans crash. » (Il tournait déjà 10 minutes sans crash avant.)
- ✅ **Après (correct)** : « Avant le correctif, la mémoire du serveur croît de 2 Mo par minute avec 4 clients ; après, elle reste stable sur 10 minutes, même charge. »
