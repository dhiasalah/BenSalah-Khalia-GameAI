"""
Documentation du projet - Jeu Mancala à 16 trous avec 4 algorithmes d'IA
"""

# JEU MANCALA À 16 TROUS - 4 ALGORITHMES D'IA

## Description du projet

Ce projet implémente le jeu Mancala à 16 trous avec 4 algorithmes de recherche différents pour l'IA :

1. **BFS (Breadth-First Search)** - Profondeur 2
   - Explore tous les états au même niveau avant de descendre plus profond
   - Garantit de trouver la solution optimale si elle existe à la profondeur limite

2. **DFS (Depth-First Search)** - Profondeur 3
   - Explore en profondeur avant de revenir en arrière
   - Plus efficace en mémoire que BFS

3. **Min-Max avec Alpha-Beta Pruning** - Profondeur 4
   - Algorithme classique pour les jeux à deux joueurs
   - Alpha-Beta pruning élimine les branches inutiles
   - Meilleur équilibre entre vitesse et qualité de jeu

4. **Iterative Deepening DFS** - Profondeur 6
   - Combine les avantages de BFS et DFS
   - Augmente progressivement la profondeur de recherche

## Règles du jeu

### Plateau de jeu
- 16 trous numérotés de 1 à 16
- Joueur 1 : trous impairs (1,3,5,7,9,11,13,15)
- Joueur 2 : trous pairs (2,4,6,8,10,12,14,16)
- Mouvement en sens horaire

### Graines et couleurs
- 3 couleurs : Rouge (R), Bleu (B), Transparent (T)
- Au départ : 2 graines de chaque couleur par trou (6 graines par trou)
- Chaque joueur contrôle 48 graines au total

### Mouvements
- **Graines rouges** : distribuées dans tous les trous
- **Graines bleues** : distribuées uniquement dans les trous adverses
- **Graines transparentes** : le joueur choisit si elles se comportent comme rouges ou bleues

### Capture des graines
- Capture s'effectue quand un trou adversaire atteint exactement 2 ou 3 graines
- La capture remonte en arrière jusqu'au premier trou qui n'a pas 2-3 graines
- Les graines capturées quittent le plateau

### Fin de partie
- Un joueur capture 49+ graines : victoire
- Les deux joueurs atteignent 40+ graines : égalité
- Moins de 10 graines restent sur le plateau : fin de partie

## Structure du projet

```
ai_game_project/
├── game_rules.py          # Définition des règles et état du jeu
├── game_engine.py         # Moteur de jeu et exécution des coups
├── ai_algorithms.py       # 4 algorithmes de recherche
├── game_manager.py        # Gestion des parties et joueurs
├── main.py               # Interface interactive
├── test_rules.py         # Tests des cas d'exemple
├── trace_detailed.py     # Traçage détaillé des coups
└── README.md             # Cette documentation
```

## Fichiers principaux

### game_rules.py
- `Color` : énumération des couleurs (RED, BLUE, TRANSPARENT)
- `GameState` : représente l'état du plateau de jeu
  - `holes` : dictionnaire des trous avec leurs graines
  - `captured_seeds` : graines capturées par joueur
  - Méthodes utilitaires pour les trous, vérification fin de partie

### game_engine.py
- `GameEngine` : exécute les mouvements et gère la capture
  - `play_move()` : exécute un coup légal
  - `_capture_seeds()` : gère la logique de capture
- `MoveGenerator` : génère les mouvements valides
  - `get_all_moves()` : liste tous les coups possibles
  - `apply_move()` : crée un nouvel état après un coup

### ai_algorithms.py
- `Evaluator` : évalue la qualité d'une position
- `BFSBot` : implémentation BFS
- `DFSBot` : implémentation DFS
- `MinMaxBot` : implémentation Min-Max avec Alpha-Beta
- `IterativeDeepeningDFSBot` : implémentation ID-DFS

### game_manager.py
- `AIPlayer` : joueur contrôlé par l'IA
- `HumanPlayer` : joueur humain (interface en console)
- `GameManager` : gère une partie complète
- `Tournament` : gère les tournois multi-joueurs

### main.py
- Interface interactive en ligne de commande
- Menus de sélection pour les modes de jeu
- Possibilité de jouer contre l'IA, IA vs IA, ou tournoi

## Comment utiliser

### Installation
```bash
cd "C:\Users\USER\Desktop\master 1\ai game programming\ai_game_project"
```

### Lancer le jeu interactif
```bash
python main.py
```

### Options disponibles
1. **Jouer contre l'IA** : Choisissez votre position et l'IA adversaire
2. **IA vs IA** : Regardez deux IA se battre
3. **Tournoi** : Tous les algorithmes s'affrontent (6 matchs)
4. **Quitter** : Quitter le programme

### Tester les règles
```bash
python test_rules.py
```

Exécute 3 cas de test basés sur les exemples fournis dans les règles.

## Formats des mouvements

Un coup est exprimé comme : `[TROU][COULEUR]`

Exemples :
- `3R` : Joue les graines rouges du trou 3
- `14B` : Joue les graines bleues du trou 14
- `5TR` : Joue les graines transparentes du trou 5 comme des rouges
- `8TB` : Joue les graines transparentes du trou 8 comme des bleues

## Analyse des algorithmes

### BFS (Profondeur 2)
- **Avantages** : Simple à comprendre, explore systématiquement
- **Inconvénients** : Peu de profondeur, décisions limitées
- **Complexité** : O(b^d) où b est le facteur de branchement

### DFS (Profondeur 3)
- **Avantages** : Meilleure profondeur qu'BFS
- **Inconvénients** : Peut être piégé dans les mauvaises branches
- **Complexité** : O(b^d) mais consomme moins de mémoire

### Min-Max + Alpha-Beta (Profondeur 4)
- **Avantages** : Joue très bien grâce à l'élagage
- **Inconvénients** : Plus lent pour les grandes profondeurs
- **Complexité** : O(b^(d/2)) en moyenne avec l'élagage

### Iterative Deepening DFS (Profondeur 6)
- **Avantages** : Combine BFS et DFS, trouvaille optimale
- **Inconvénients** : Re-explore les niveaux précédents
- **Complexité** : O(b^d) mais avec exploration progressive

## Exemple de partie

```
Plateau initial : 6 graines par trou (2R, 2B, 2T)

Joueur 1 joue : Trou 3, Couleur Rouge
→ Distribution des graines rouges vers les trous 4,5,6

Joueur 2 joue : Trou 14, Couleur Bleu
→ Distribution uniquement dans les trous adverses
→ Si capture : accumulation des graines

...
```

## Notes sur la mise en œuvre

- L'état du jeu est immuable (copy() crée une nouvelle instance)
- Les algorithmes utilisent la programmation récursive
- L'évaluation des positions se base sur la différence de graines
- Gestion correcte de la circularité du plateau (trou 16 → trou 1)

## Améliorations possibles

1. Ajouter une interface graphique (PyGame, Tkinter)
2. Implémenter la mémorisation (transposition tables) pour Min-Max
3. Ajouter d'autres heuristiques d'évaluation
4. Implémenter Monte Carlo Tree Search (MCTS)
5. Optimiser les performances avec Cython
6. Ajouter la sauvegarde/chargement de parties

## Auteur

Développé comme projet d'IA pour jeux avec algorithmes de recherche.

## Licence

Libre d'utilisation à titre éducatif.

