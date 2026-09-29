---
description: Un garde-fou s'exécute avant l'écriture qu'il protège ; un contrôle placé après n'est qu'une alarme
trigger: always_on
---

# RULE : Un garde-fou s'exécute AVANT l'écriture qu'il protège — une vérification placée après n'est qu'une alarme

## Règle à appliquer

1. **Pour chaque garde-fou, nommer l'écriture qu'il protège, puis vérifier qu'il s'exécute AVANT elle**, dans l'ordre réel d'exécution. Si l'écriture est déjà faite quand le contrôle échoue, c'est une alarme, et on l'appelle ainsi.
2. **Quand l'écriture ne peut pas être précédée du contrôle**, écrire d'abord dans un emplacement que personne ne consomme (buffer temporaire, fichier temporaire, étape de CI séparée), vérifier, puis seulement publier.
3. **Un code repris tel quel n'exempte pas de relire l'ordre.** Un défaut trouvé dans du code copié se signale.
4. **Dans une PR, le mot « garde-fou » est réservé à un contrôle qui bloque l'écriture.**

## Exemple

- ❌ **Avant (incorrect)** : copier le payload dans le composant de l'entité, puis vérifier que l'id d'entité existe — le composant d'une autre entité a déjà été écrasé.
- ✅ **Après (correct)** : vérifier l'existence de l'entité et la taille du payload, puis seulement écrire le composant.
