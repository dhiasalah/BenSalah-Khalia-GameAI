# Jeu Mancala à 16 Trous - Version C++

## Description
Jeu de Mancala à 16 trous avec 4 IA utilisant différents algorithmes de recherche:
- BFS (Breadth-First Search) - Profondeur 2
- DFS (Depth-First Search) - Profondeur 3
- Min-Max - Profondeur 4
- Alpha-Beta Pruning - Profondeur 5 (Recommandé)
- Iterative Deepening DFS - Profondeur 6

## Règles du jeu
- 16 trous numérotés de 1 à 16
- Joueur 1: trous impairs (1,3,5,7,9,11,13,15)
- Joueur 2: trous pairs (2,4,6,8,10,12,14,16)
- Au départ: 2 graines rouges, 2 bleues, 2 transparentes par trou
- Trois couleurs: Red (R), Blue (B), Transparent (T)

## Compilation

### Avec g++ (MinGW sur Windows ou GCC sur Linux/Mac)
```bash
g++ -std=c++17 -O2 -o mancala main.cpp
```

### Avec CMake
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Avec Visual Studio
Ouvrez le projet dans Visual Studio et compilez avec C++17 activé.

## Exécution
```bash
./mancala
```

## Structure des fichiers
- `config.h` - Configuration du jeu et des IA
- `game_rules.h` - Règles du jeu et état du plateau
- `game_engine.h` - Moteur de jeu (exécution des coups, captures)
- `ai_algorithms.h` - Algorithmes d'IA (BFS, DFS, MinMax, Alpha-Beta, IDDFS)
- `game_manager.h` - Gestionnaire de parties et joueurs
- `main.cpp` - Interface utilisateur et menu principal

## Format des coups
- `14B` : Trou 14, couleur BLEU
- `3R` : Trou 3, couleur ROUGE
- `5TR` : Trou 5, graines TRANSPARENTES comme ROUGE
- `8TB` : Trou 8, graines TRANSPARENTES comme BLEU

## Conditions de fin de partie
- Un joueur capture 49+ graines → Victoire
- Les deux joueurs capturent 40+ graines → Égalité
- Moins de 10 graines sur le plateau → Fin
- 400 coups atteints → Celui avec le plus de graines gagne
