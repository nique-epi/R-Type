---
description: Un script d'édition multi-fichiers vérifie toutes ses ancres en mémoire avant d'écrire le premier fichier
trigger: always_on
---

# RULE : Un script d'édition multi-fichiers valide toutes ses ancres avant d'écrire le premier fichier

## Règle à appliquer

1. **Deux phases, jamais entrelacées** : charger tous les fichiers en mémoire et vérifier toutes les ancres (présence, nombre exact d'occurrences), puis écrire — seulement si toutes les vérifications ont passé.
2. **Mesurer un compte avant de l'asserter** : `grep -c '<ancre>' <fichier>` sur le fichier réel, puis reporter ce chiffre dans le script.
3. **Si un script d'édition échoue quand même** : `git status --short` d'abord, pour savoir exactement ce qui a été écrit, puis ne rejouer que le reste. Ne jamais relancer le script entier sur un arbre partiellement modifié.

## Exemple

- ❌ **Avant (incorrect)** : un script qui renomme `PacketMgr` dans six fichiers en écrivant au fil de l'eau, et échoue au quatrième sur une ancre devinée → trois fichiers modifiés, trois non, le build casse.
- ✅ **Après (correct)** : `grep -c 'PacketMgr'` sur chaque fichier → le script vérifie les six ancres en mémoire, puis écrit les six fichiers d'un coup ; build et `grep -rn PacketMgr src` vide pour conclure.
