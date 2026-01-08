/**
 * Configuration du Bot Awale pour awale-arena
 * Modifiez ces paramètres selon vos besoins
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== PARAMÈTRES IA ====================

// Profondeur de recherche Minimax
// 2: très rapide (~50-100ms), moins bon stratégiquement
// 3: équilibré (~200-500ms), recommandé pour la plateforme ✓
// 4: lent (~1-2s), meilleure qualité
// 5+: très lent, pas recommandé
const int AI_DEPTH = 3;

// Poids dans l'évaluation des positions
const int SCORE_WEIGHT = 10;      // Poids pour la différence de graines capturées
const int BOARD_WEIGHT = 2;       // Poids pour la différence de graines sur le plateau

// ==================== DEBUG ====================

// Afficher les détails des calculs IA (pour développement)
const bool DEBUG_MODE = false;

// Afficher l'état du plateau après chaque coup
const bool SHOW_BOARD_STATE = false;

// ==================== TIMING ====================

// Temps maximum par coup (en secondes) - à adapter selon la plateforme
const double MAX_TIME_PER_MOVE = 3.0;

// ==================== STRATÉGIE ====================

// Stratégies disponibles:
// - "random": choix aléatoire parmi les coups valides
// - "greedy": meilleur coup immédiat sans anticipation
// - "minimax": algorithme Minimax complet (défaut)
// const char* STRATEGY = "minimax";

// ==================== LOGGING ====================

// Enregistrer les mouvements dans un fichier (pour debug)
const bool LOG_MOVES = false;
// const char* LOG_FILE = "bot_moves.log";

// ==================== PLATEFORME ====================

// URL de la plateforme (informations uniquement)
// const char* PLATFORM_URL = "https://awale-arena.colindeseroux.fr/";

// Version du bot
// const char* BOT_VERSION = "1.0.0";
// const char* BOT_NAME = "AI-Awale-MinMax";

// ==================== FORMAT DES COUPS ====================

// Format: NC ou NTC
// N: numéro du trou (1-16)
// C: couleur (R=rouge, B=bleu)
// T: optionnel, indique transparent
//
// Exemples:
// - "1R": graines rouges du trou 1
// - "3B": graines bleues du trou 3
// - "4TR": graines transparentes du trou 4 comme rouges
// - "5TB": graines transparentes du trou 5 comme bleues

// ==================== RÈGLES DU JEU ====================

// Nombre de trous par joueur
const int HOLES_PER_PLAYER = 8;

// Nombre total de trous
const int TOTAL_HOLES = 16;

// Graines initiales par couleur par trou
const int INITIAL_SEEDS_PER_COLOR = 2;

// Nombre total de graines par trou au départ
const int INITIAL_SEEDS_PER_HOLE = INITIAL_SEEDS_PER_COLOR * 3;

// Nombre total de graines au départ
const int TOTAL_INITIAL_SEEDS = TOTAL_HOLES * INITIAL_SEEDS_PER_HOLE;

// Points de capture: exactement 2 ou 3 graines
const int CAPTURE_MIN = 2;
const int CAPTURE_MAX = 3;

// Fin de partie
const int WINNING_SEEDS = 49;          // Graine pour victoire
const int DRAW_THRESHOLD = 40;         // Égalité si les deux joueurs ont 40+
const int GAME_END_SEEDS = 10;         // Fin si moins de 10 graines sur le plateau

// Joueurs
const int PLAYER_1 = 0;
const int PLAYER_2 = 1;

#endif // CONFIG_H
