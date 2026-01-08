"""
Algorithmes d'IA pour le jeu Mancala
- BFS (Breadth-First Search)
- DFS (Depth-First Search)
- Min-Max avec Alpha-Beta Pruning
- Iterative Deepening DFS
"""

from game_rules import GameState, Color
from game_engine import GameEngine, MoveGenerator
from typing import List, Tuple, Dict, Optional
from collections import deque
import sys

class Evaluator:
    """Évalue la qualité d'une position"""

    @staticmethod
    def evaluate(state: GameState, player: int) -> float:
        """
        Évalue une position pour un joueur
        Score positif = avantage pour le joueur
        Score négatif = avantage pour l'adversaire
        """
        opponent = 3 - player

        # Différence de graines capturées
        score = (state.captured_seeds[player] - state.captured_seeds[opponent]) * 10

        # Bonus pour les graines sur le plateau
        player_seeds = 0
        opponent_seeds = 0

        for hole in state.get_player_holes(player):
            player_seeds += state.get_total_seeds(hole)

        for hole in state.get_player_holes(opponent):
            opponent_seeds += state.get_total_seeds(hole)

        score += (player_seeds - opponent_seeds) * 2

        return float(score)

    @staticmethod
    def is_terminal(state: GameState) -> bool:
        """Vérifie si c'est un état terminal"""
        return state.is_game_over()

    @staticmethod
    def get_terminal_score(state: GameState, player: int) -> float:
        """Retourne le score d'un état terminal"""
        if state.captured_seeds[player] > state.captured_seeds[3 - player]:
            return float('inf')  # Victoire
        elif state.captured_seeds[player] < state.captured_seeds[3 - player]:
            return float('-inf')  # Défaite
        else:
            return 0.0  # Égalité


class BFSBot:
    """Algorithme BFS pour explorer les états à profondeur égale"""

    def __init__(self, depth: int = 2):
        self.depth = depth
        self.evaluator = Evaluator()

    def search(self, state: GameState, player: int) -> Optional[Tuple[int, Color, Color]]:
        """
        Effectue une recherche BFS jusqu'à une profondeur donnée
        Retourne le meilleur coup
        """
        if self.evaluator.is_terminal(state):
            return None

        # Queue: (state, depth, move_path)
        queue = deque([(state, 0, [])])
        best_move = None
        best_score = float('-inf')

        while queue:
            current_state, depth, moves = queue.popleft()

            # Si on a atteint la profondeur désirée
            if depth == self.depth:
                score = self.evaluator.evaluate(current_state, player)
                if score > best_score and len(moves) > 0:
                    best_score = score
                    best_move = moves[0]
                continue

            # Explorer les enfants
            if current_state.current_player == player:
                next_moves = MoveGenerator.get_all_moves(current_state, player)

                for hole, color, transparent_as in next_moves:
                    new_state = MoveGenerator.apply_move(current_state, hole, color, transparent_as)
                    new_moves = moves + [(hole, color, transparent_as)]
                    queue.append((new_state, depth + 1, new_moves))

        return best_move

    def get_move(self, state: GameState, player: int) -> Optional[Tuple[int, Color, Color]]:
        """Interface publique pour obtenir un coup"""
        return self.search(state, player)


class DFSBot:
    """Algorithme DFS pour explorer les états en profondeur"""

    def __init__(self, depth: int = 3):
        self.depth = depth
        self.evaluator = Evaluator()
        self.visited = set()

    def search(self, state: GameState, player: int, depth: int = 0) -> Tuple[float, Optional[Tuple[int, Color, Color]]]:
        """
        Effectue une recherche DFS
        Retourne (meilleur_score, meilleur_coup)
        """
        # État terminal
        if self.evaluator.is_terminal(state):
            opponent = 3 - player
            if state.captured_seeds[player] > state.captured_seeds[opponent]:
                return (float('inf'), None)
            elif state.captured_seeds[player] < state.captured_seeds[opponent]:
                return (float('-inf'), None)
            else:
                return (0.0, None)

        # Profondeur atteinte
        if depth >= self.depth:
            return (self.evaluator.evaluate(state, player), None)

        best_score = float('-inf')
        best_move = None

        if state.current_player == player:
            # Joueur maximisant
            moves = MoveGenerator.get_all_moves(state, player)

            for hole, color, transparent_as in moves:
                new_state = MoveGenerator.apply_move(state, hole, color, transparent_as)
                score, _ = self.search(new_state, player, depth + 1)

                if score > best_score:
                    best_score = score
                    best_move = (hole, color, transparent_as)
        else:
            # Joueur minimisant
            opponent = 3 - player
            moves = MoveGenerator.get_all_moves(state, opponent)
            best_score = float('inf')

            for hole, color, transparent_as in moves:
                new_state = MoveGenerator.apply_move(state, hole, color, transparent_as)
                score, _ = self.search(new_state, player, depth + 1)

                if score < best_score:
                    best_score = score
                    best_move = (hole, color, transparent_as)

        return (best_score, best_move)

    def get_move(self, state: GameState, player: int) -> Optional[Tuple[int, Color, Color]]:
        """Interface publique pour obtenir un coup"""
        _, move = self.search(state, player, 0)
        return move


class MinMaxBot:
    """Algorithme Min-Max avec Alpha-Beta Pruning"""

    def __init__(self, depth: int = 4):
        self.depth = depth
        self.evaluator = Evaluator()
        self.nodes_explored = 0

    def search(self, state: GameState, player: int, depth: int = 0,
               alpha: float = float('-inf'), beta: float = float('inf')) -> Tuple[float, Optional[Tuple[int, Color, Color]]]:
        """
        Min-Max avec Alpha-Beta Pruning
        """
        self.nodes_explored += 1

        # État terminal
        if self.evaluator.is_terminal(state):
            return self.evaluator.get_terminal_score(state, player), None

        # Profondeur atteinte
        if depth >= self.depth:
            return self.evaluator.evaluate(state, player), None

        if state.current_player == player:
            # Nœud maximisant
            max_eval = float('-inf')
            best_move = None

            moves = MoveGenerator.get_all_moves(state, player)

            for hole, color, transparent_as in moves:
                new_state = MoveGenerator.apply_move(state, hole, color, transparent_as)
                eval_score, _ = self.search(new_state, player, depth + 1, alpha, beta)

                if eval_score > max_eval:
                    max_eval = eval_score
                    best_move = (hole, color, transparent_as)

                alpha = max(alpha, eval_score)
                if beta <= alpha:
                    break  # Beta cutoff

            return max_eval, best_move
        else:
            # Nœud minimisant
            opponent = 3 - player
            min_eval = float('inf')
            best_move = None

            moves = MoveGenerator.get_all_moves(state, opponent)

            for hole, color, transparent_as in moves:
                new_state = MoveGenerator.apply_move(state, hole, color, transparent_as)
                eval_score, _ = self.search(new_state, player, depth + 1, alpha, beta)

                if eval_score < min_eval:
                    min_eval = eval_score
                    best_move = (hole, color, transparent_as)

                beta = min(beta, eval_score)
                if beta <= alpha:
                    break  # Alpha cutoff

            return min_eval, best_move

    def get_move(self, state: GameState, player: int) -> Optional[Tuple[int, Color, Color]]:
        """Interface publique pour obtenir un coup"""
        self.nodes_explored = 0
        _, move = self.search(state, player, 0)
        return move


class AlphaBetaBot:
    """Algorithme Alpha-Beta Pruning - Version optimisée de MinMax"""

    def __init__(self, depth: int = 5):
        self.depth = depth
        self.evaluator = Evaluator()
        self.nodes_explored = 0
        self.pruned_branches = 0

    def search(self, state: GameState, player: int, depth: int = 0,
               alpha: float = float('-inf'), beta: float = float('inf'),
               is_maximizing: bool = True) -> Tuple[float, Optional[Tuple[int, Color, Color]]]:
        """
        Alpha-Beta Pruning - Élagage des branches non prometteuses

        Alpha: Meilleure valeur pour le joueur maximisant
        Beta: Meilleure valeur pour le joueur minimisant
        """
        self.nodes_explored += 1

        # État terminal
        if self.evaluator.is_terminal(state):
            terminal_score = self.evaluator.get_terminal_score(state, player)
            return terminal_score, None

        # Profondeur atteinte
        if depth >= self.depth:
            return self.evaluator.evaluate(state, player), None

        current_player = state.current_player

        if is_maximizing:
            # Nœud maximisant (notre joueur)
            max_eval = float('-inf')
            best_move = None

            moves = MoveGenerator.get_all_moves(state, current_player)

            for hole, color, transparent_as in moves:
                new_state = MoveGenerator.apply_move(state, hole, color, transparent_as)

                # Déterminer si le prochain niveau est maximisant ou minimisant
                next_is_maximizing = (new_state.current_player == player)

                eval_score, _ = self.search(new_state, player, depth + 1, alpha, beta, next_is_maximizing)

                if eval_score > max_eval:
                    max_eval = eval_score
                    best_move = (hole, color, transparent_as)

                # Mise à jour d'alpha
                alpha = max(alpha, eval_score)

                # Élagage Beta
                if beta <= alpha:
                    self.pruned_branches += 1
                    break  # Beta cutoff - on peut arrêter d'explorer cette branche

            return max_eval, best_move
        else:
            # Nœud minimisant (adversaire)
            min_eval = float('inf')
            best_move = None

            moves = MoveGenerator.get_all_moves(state, current_player)

            for hole, color, transparent_as in moves:
                new_state = MoveGenerator.apply_move(state, hole, color, transparent_as)

                # Déterminer si le prochain niveau est maximisant ou minimisant
                next_is_maximizing = (new_state.current_player == player)

                eval_score, _ = self.search(new_state, player, depth + 1, alpha, beta, next_is_maximizing)

                if eval_score < min_eval:
                    min_eval = eval_score
                    best_move = (hole, color, transparent_as)

                # Mise à jour de beta
                beta = min(beta, eval_score)

                # Élagage Alpha
                if beta <= alpha:
                    self.pruned_branches += 1
                    break  # Alpha cutoff - on peut arrêter d'explorer cette branche

            return min_eval, best_move

    def get_move(self, state: GameState, player: int) -> Optional[Tuple[int, Color, Color]]:
        """Interface publique pour obtenir un coup"""
        self.nodes_explored = 0
        self.pruned_branches = 0

        _, move = self.search(state, player, 0, float('-inf'), float('inf'), True)

        # print(f"[Alpha-Beta] Nœuds explorés: {self.nodes_explored}, Branches élaguées: {self.pruned_branches}")

        return move


class IterativeDeepeningDFSBot:
    """Algorithme Iterative Deepening DFS"""

    def __init__(self, max_depth: int = 6):
        self.max_depth = max_depth
        self.evaluator = Evaluator()
        self.nodes_explored = 0

    def dfs_limited(self, state: GameState, player: int, depth_limit: int, depth: int = 0) -> Tuple[float, Optional[Tuple[int, Color, Color]]]:
        """
        DFS avec limite de profondeur
        """
        self.nodes_explored += 1

        # État terminal
        if self.evaluator.is_terminal(state):
            return self.evaluator.get_terminal_score(state, player), None

        # Limite de profondeur atteinte
        if depth >= depth_limit:
            return self.evaluator.evaluate(state, player), None

        best_score = float('-inf')
        best_move = None

        if state.current_player == player:
            # Maximisant
            moves = MoveGenerator.get_all_moves(state, player)

            for hole, color, transparent_as in moves:
                new_state = MoveGenerator.apply_move(state, hole, color, transparent_as)
                score, _ = self.dfs_limited(new_state, player, depth_limit, depth + 1)

                if score > best_score:
                    best_score = score
                    best_move = (hole, color, transparent_as)
        else:
            # Minimisant
            opponent = 3 - player
            moves = MoveGenerator.get_all_moves(state, opponent)
            best_score = float('inf')

            for hole, color, transparent_as in moves:
                new_state = MoveGenerator.apply_move(state, hole, color, transparent_as)
                score, _ = self.dfs_limited(new_state, player, depth_limit, depth + 1)

                if score < best_score:
                    best_score = score
                    best_move = (hole, color, transparent_as)

        return best_score, best_move

    def search(self, state: GameState, player: int) -> Optional[Tuple[int, Color, Color]]:
        """
        Iterative Deepening: augmente progressivement la profondeur
        """
        best_move = None

        for depth in range(1, self.max_depth + 1):
            self.nodes_explored = 0
            _, move = self.dfs_limited(state, player, depth)

            if move is not None:
                best_move = move
                # print(f"[ID-DFS] Profondeur {depth}: {self.nodes_explored} nœuds explorés")

        return best_move

    def get_move(self, state: GameState, player: int) -> Optional[Tuple[int, Color, Color]]:
        """Interface publique pour obtenir un coup"""
        return self.search(state, player)

