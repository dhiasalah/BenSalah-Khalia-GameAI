"""
Configuration du Bot Awale pour awale-arena
Modifiez ces paramètres selon vos besoins
"""

# ==================== PARAMÈTRES IA ====================

# Profondeur de recherche Minimax
# 2: très rapide (~50-100ms), moins bon stratégiquement
# 3: équilibré (~200-500ms), recommandé pour la plateforme ✓
# 4: lent (~1-2s), meilleure qualité
# 5+: très lent, pas recommandé
AI_DEPTH = 3

# Poids dans l'évaluation des positions
SCORE_WEIGHT = 10      # Poids pour la différence de graines capturées
BOARD_WEIGHT = 2       # Poids pour la différence de graines sur le plateau

# ==================== DEBUG ====================

# Afficher les détails des calculs IA (pour développement)
DEBUG_MODE = False

# Afficher l'état du plateau après chaque coup
SHOW_BOARD_STATE = False

# ==================== TIMING ====================

# Temps maximum par coup (en secondes) - à adapter selon la plateforme
MAX_TIME_PER_MOVE = 3.0

# ==================== STRATÉGIE ====================

# Stratégies disponibles:
# - "random": choix aléatoire parmi les coups valides
# - "greedy": meilleur coup immédiat sans anticipation
# - "minimax": algorithme Minimax complet (défaut)
STRATEGY = "minimax"

# ==================== LOGGING ====================

# Enregistrer les mouvements dans un fichier (pour debug)
LOG_MOVES = False
LOG_FILE = "bot_moves.log"

# ==================== PLATEFORME ====================

# URL de la plateforme (informations uniquement)
PLATFORM_URL = "https://awale-arena.colindeseroux.fr/"

# Version du bot
BOT_VERSION = "1.0.0"
BOT_NAME = "AI-Awale-MinMax"

# ==================== FORMAT DES COUPS ====================

# Format: NC ou NTC
# N: numéro du trou (1-16)
# C: couleur (R=rouge, B=bleu)
# T: optionnel, indique transparent
#
# Exemples:
# - "1R": graines rouges du trou 1
# - "3B": graines bleues du trou 3
# - "4TR": graines transparentes du trou 4 comme rouges
# - "5TB": graines transparentes du trou 5 comme bleues

VALID_COLORS = ['R', 'B', 'T']
VALID_HOLES = list(range(1, 17))  # 1 à 16

# ==================== RÈGLES DU JEU ====================

# Nombre de trous par joueur
HOLES_PER_PLAYER = 8

# Nombre total de trous
TOTAL_HOLES = 16

# Graines initiales par couleur par trou
INITIAL_SEEDS_PER_COLOR = 2

# Nombre total de graines par trou au départ
INITIAL_SEEDS_PER_HOLE = INITIAL_SEEDS_PER_COLOR * 3

# Nombre total de graines au départ
TOTAL_INITIAL_SEEDS = TOTAL_HOLES * INITIAL_SEEDS_PER_HOLE

# Points de capture: exactement 2 ou 3 graines
CAPTURE_MIN = 2
CAPTURE_MAX = 3

# Fin de partie
WINNING_SEEDS = 49          # Graine pour victoire
DRAW_THRESHOLD = 40         # Égalité si les deux joueurs ont 40+
GAME_END_SEEDS = 10         # Fin si moins de 10 graines sur le plateau

# Joueurs
PLAYER_1 = 0
PLAYER_2 = 1
PLAYERS = [PLAYER_1, PLAYER_2]

# Trous des joueurs
PLAYER_1_HOLES = [i for i in range(16) if i % 2 == 0]  # indices 0,2,4,6,8,10,12,14 -> trous 1,3,5,7,9,11,13,15
PLAYER_2_HOLES = [i for i in range(16) if i % 2 == 1]  # indices 1,3,5,7,9,11,13,15 -> trous 2,4,6,8,10,12,14,16

