---
description: Ne jamais raisonner ni décider sur le contenu d'un fichier qui n'a pas été réellement lu
trigger: always_on
---

# RULE : Ne jamais raisonner sur le contenu d'un fichier non réellement lu

## Règle à appliquer

1. **Aucune affirmation ni décision sur un fichier sans une lecture qui a réellement renvoyé son contenu.** Un retour « File does not exist », vide ou en erreur = STOP : relocaliser le fichier (`find`, `git ls-files`) avant de continuer. Ne jamais combler le contenu manquant par déduction.
2. **Vérifier la stack réelle sur les fichiers du projet** avant d'appliquer un savoir général : version de SFML (3, pas 2), d'Asio (standalone, pas Boost.Asio), standard C++ (20), options du `CMakeLists.txt`. Suivre le code réel ; n'emprunter au savoir général que ce qui est vérifié compatible.
3. **Ne jamais batcher dans le même bloc un appel faillible** (glob sans correspondance, commande au code de sortie non nul) **avec des écritures** (`Write`, `Edit`, `rm`, `git commit`). Séparer la reconnaissance de l'écriture.

## Exemple

- ❌ **Avant (incorrect)** : la lecture de `EntityRegistry.hpp` échoue → je continue en supposant une méthode `getComponent<T>(entity)` et je code dessus.
- ✅ **Après (correct)** : la lecture échoue → `git ls-files | grep -i registry` → lecture du vrai fichier → je constate `get<T>(entity)` retournant un `std::optional` → je code sur le réel.
