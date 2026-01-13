# Jeu Mancala à 16 Trous - Version C++

## Compilation

### 1. Compiler le bot C++

Compilez votre bot C++ avec g++:

```bash
cd c_version
g++ -o BenSalah-Khalia-GameAI.exe bot.cpp -std=c++17 -O2
```

### 2. Compiler l'arbitre Java

Compilez les fichiers Java:

```bash
cd ..
javac Arbitre.java
```

## Test et Exécution

### Tester votre bot

Lancez une partie avec l'arbitre:

```bash
java Arbitre
```

L'arbitre lancera automatiquement les deux bots configurés et gérera la partie.

### Configuration des bots

Dans `Arbitre.java` (lignes 10-15), les bots sont configurés ainsi:

```java
Process A = Runtime.getRuntime().exec(new String[]{"ikram.exe", "JoueurA"});
Process B = Runtime.getRuntime().exec(new String[]{"c_version\\BenSalah-Khalia-GameAI.exe", "JoueurB"});
```

#### Comment changer les joueurs

Pour jouer contre un autre bot, modifiez les lignes dans `Arbitre.java`:

```java
// Joueur 1 (trous impairs 1,3,5,7,9,11,13,15)
Process A = Runtime.getRuntime().exec(new String[]{"bot1.exe", "JoueurA"});

// Joueur 2 (trous pairs 2,4,6,8,10,12,14,16)
Process B = Runtime.getRuntime().exec(new String[]{"bot2.exe", "JoueurB"});
```

Exemples:

- **Votre bot vs Ikram**: `{"c_version\\BenSalah-Khalia-GameAI.exe", "JoueurA"}` vs `{"ikram.exe", "JoueurB"}`
- **Votre bot vs Kacem**: `{"c_version\\BenSalah-Khalia-GameAI.exe", "JoueurA"}` vs `{"kacem.exe", "JoueurB"}`

**Important**: Les bots doivent accepter un argument (JoueurA ou JoueurB) et suivre le protocole de l'arbitre.

## Structure des fichiers

### C++ Bot

- `bot.cpp` - Bot principal utilisant MinMax avec iterative deepening
- `ai_algorithms.h` - Algorithmes d'IA (MinMax, Alpha-Beta)
- `game_engine.h` - Moteur de jeu (exécution des coups, captures)
- `game_rules.h` - Règles du jeu et état du plateau
- `game_manager.h` - Gestionnaire de parties
- `config.h` - Configuration du jeu
- `main.cpp` - Jeu standalone avec interface utilisateur

### Java Arbitre

- `Arbitre.java` - Arbitre gérant les parties entre deux bots
- `JoueurExterne.java` - Interface pour joueurs externes
