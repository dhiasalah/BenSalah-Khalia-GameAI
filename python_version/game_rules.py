"""
Règles du jeu Mancala à 16 trous
- 16 trous (8 par joueur)
- Numérotés de 1 à 16, en sens horaire
- Joueur 1: trous impairs (1,3,5,7,9,11,13,15)
- Joueur 2: trous pairs (2,4,6,8,10,12,14,16)
- Au départ: 2 graines rouges, 2 bleues, 2 transparentes par trou
- Trois couleurs: Red (R), Blue (B), Transparent (T)
"""

from enum import Enum
from typing import List, Dict, Tuple, Set

class Color(Enum):
    RED = "R"
    BLUE = "B"
    TRANSPARENT = "T"

# Pre-computed player holes (avoid repeated list creation)
_PLAYER1_HOLES = (1, 3, 5, 7, 9, 11, 13, 15)
_PLAYER2_HOLES = (2, 4, 6, 8, 10, 12, 14, 16)

class GameState:
    """État du jeu - Optimized version"""

    __slots__ = ['holes', 'captured_seeds', 'current_player', 'move_count']
    
    MAX_MOVES = 400  # Limite de 400 coups (200 par joueur)

    def __init__(self):
        # Dictionnaire des trous: {numéro: {couleur: nombre}}
        self.holes: Dict[int, Dict[Color, int]] = {}
        self.initialize_board()
        self.captured_seeds = {1: 0, 2: 0}  # Graines capturées par joueur
        self.current_player = 1
        self.move_count = 0  # Compteur de coups joués

    def initialize_board(self):
        """Initialise le plateau avec 2 graines de chaque couleur par trou"""
        for hole in range(1, 17):
            self.holes[hole] = {
                Color.RED: 2,
                Color.BLUE: 2,
                Color.TRANSPARENT: 2
            }

    def get_player_holes(self, player: int) -> Tuple[int, ...]:
        """Retourne les trous contrôlés par un joueur
        Joueur 1: trous impairs, Joueur 2: trous pairs"""
        return _PLAYER1_HOLES if player == 1 else _PLAYER2_HOLES

    def get_total_seeds(self, hole: int) -> int:
        """Retourne le nombre total de graines dans un trou"""
        h = self.holes[hole]
        return h[Color.RED] + h[Color.BLUE] + h[Color.TRANSPARENT]

    def get_seeds_on_board(self) -> int:
        """Retourne le nombre total de graines sur le plateau"""
        total = 0
        for hole in self.holes.values():
            total += sum(hole.values())
        return total

    def is_game_over(self) -> bool:
        """
        Vérifie si le jeu est terminé selon les règles:
        - Un joueur a capturé 49+ graines -> victoire
        - Les deux joueurs ont capturé 40+ graines -> égalité
        - Strictement moins de 10 graines restent sur le plateau -> fin
        - 400 coups atteints -> fin (celui avec le plus de graines gagne)
        """
        # Condition 0: Limite de 400 coups atteinte
        if self.move_count >= 400:
            return True

        c1, c2 = self.captured_seeds[1], self.captured_seeds[2]

        # Condition 2: Un joueur a capturé 49+ graines (victoire)
        if c1 >= 49 or c2 >= 49:
            return True

        # Condition 3: Les deux joueurs ont capturé 40+ graines (égalité)
        if c1 >= 40 and c2 >= 40:
            return True

        # Condition 1: Moins de 10 graines sur le plateau (fast calc)
        if 96 - c1 - c2 < 10:
            return True

        return False

    def get_winner(self) -> int:
        """
        Retourne le gagnant: 1, 2 ou 0 (égalité)
        Règles:
        - Joueur avec 49+ graines gagne
        - Si les deux ont 40+: égalité
        - Si moins de 10 graines: celui avec le plus de graines gagne
        - Sinon: celui avec le plus de graines gagne
        """
        # Si un joueur a 49+, il gagne
        if self.captured_seeds[1] >= 49:
            return 1
        if self.captured_seeds[2] >= 49:
            return 2

        # Sinon, compare les scores
        if self.captured_seeds[1] > self.captured_seeds[2]:
            return 1
        elif self.captured_seeds[2] > self.captured_seeds[1]:
            return 2
        else:
            return 0  # Égalité

    def get_valid_moves(self, player: int) -> List[Tuple[int, Color]]:
        """Retourne les coups valides pour un joueur
        Format: (numéro_trou, couleur)"""
        valid_moves = []
        player_holes = self.get_player_holes(player)

        for hole in player_holes:
            for color in Color:
                if self.holes[hole][color] > 0:
                    valid_moves.append((hole, color))

        return valid_moves

    def copy(self) -> 'GameState':
        """Crée une copie de l'état du jeu - Optimized"""
        new_state = object.__new__(GameState)
        new_state.holes = {h: self.holes[h].copy() for h in range(1, 17)}
        new_state.captured_seeds = {1: self.captured_seeds[1], 2: self.captured_seeds[2]}
        new_state.current_player = self.current_player
        new_state.move_count = self.move_count
        return new_state

    def __str__(self) -> str:
        """Affichage du plateau"""
        result = "\n" + "="*80 + "\n"
        result += f"Player 1 captured: {self.captured_seeds[1]} seeds\n"
        result += f"Player 2 captured: {self.captured_seeds[2]} seeds\n"
        result += f"Current player: {self.current_player}\n"
        result += "="*80 + "\n"

        # Affichage du plateau
        result += "Holes 16-15-14-13-12-11-10-9\n"
        line = ""
        for h in range(16, 8, -1):
            line += f"{h:2d}({sum(self.holes[h].values()):2d}) "
        result += line + "\n"

        line = ""
        for h in range(16, 8, -1):
            seed_repr = f"R:{self.holes[h][Color.RED]} B:{self.holes[h][Color.BLUE]} T:{self.holes[h][Color.TRANSPARENT]}"
            line += f"{seed_repr:25s}"
        result += line + "\n"

        result += "\nHoles 1-2-3-4-5-6-7-8\n"
        line = ""
        for h in range(1, 9):
            line += f"{h:2d}({sum(self.holes[h].values()):2d}) "
        result += line + "\n"

        line = ""
        for h in range(1, 9):
            seed_repr = f"R:{self.holes[h][Color.RED]} B:{self.holes[h][Color.BLUE]} T:{self.holes[h][Color.TRANSPARENT]}"
            line += f"{seed_repr:25s}"
        result += line + "\n"

        return result
