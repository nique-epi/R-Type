---
description: Un code dont le résultat doit tomber dans une plage se vérifie sur l'entrée vide, pleine et aux deux bornes — jamais sur le seul cas nominal
trigger: always_on
---

# RULE : Un contrat à plage bornée se vérifie sur l'état VIDE et aux deux bornes — jamais sur le seul cas nominal

## Règle à appliquer

Avant de livrer un code dont le résultat doit rester **dans une plage** — index dans un buffer, offset de lecture dans un paquet, identifiant d'entité, slot de joueur, position dans une file circulaire :

1. **Dérouler à la main les cas de bornes** : entrée **vide**, entrée **pleine**, et un cran de part et d'autre de chaque borne. Écrire le résultat attendu pour chacun avant de coder. La plupart de ces contrats cassent sur le vide, jamais sur le cas nominal.
2. **Borner explicitement sur la donnée dont on répond**, plutôt que de compter sur un invariant supposé entre deux longueurs indépendantes (taille annoncée dans l'en-tête vs octets reçus).
3. **Écrire le test de bornes, le voir échouer sur le code fautif, puis le garder.**

## Exemple

- ❌ **Avant (incorrect)** : `readUint16(buffer, offset)` testé sur un paquet complet de 64 octets uniquement → lecture hors borne sur un datagramme de 1 octet.
- ✅ **Après (correct)** : tests sur 0, 1, taille d'en-tête − 1, taille d'en-tête et taille maximale ; la couche de validation rejette tout ce qui est plus court que l'en-tête, test vérifié rouge sur l'ancienne version.
