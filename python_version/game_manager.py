"""
Gestionnaire de jeu - Gère les parties et l'interaction entre joueurs et IA
"""

from game_rules import GameState, Color
from game_engine import GameEngine, MoveGenerator
from ai_algorithms import BFSBot, DFSBot, MinMaxBot, AlphaBetaBot, IterativeDeepeningDFSBot
from config import MAX_TIME_PER_MOVE
from typing import Optional, Tuple
import time

class AIPlayer:
    """Représente un joueur IA"""

    def __init__(self, player_id: int, algorithm_name: str, depth: int = 3):
        self.player_id = player_id
        self.algorithm_name = algorithm_name
        self.depth = depth
        self.ai = self._create_ai(algorithm_name, depth)

    def _create_ai(self, algorithm_name: str, depth: int):
        """Crée l'instance d'IA appropriée"""
        if algorithm_name.lower() == "bfs":
            return BFSBot(depth)
        elif algorithm_name.lower() == "dfs":
            return DFSBot(depth)
        elif algorithm_name.lower() == "minimax":
            return MinMaxBot(depth)
        elif algorithm_name.lower() == "alphabeta":
            return AlphaBetaBot(depth)
        elif algorithm_name.lower() == "iddfs":
            return IterativeDeepeningDFSBot(depth)
        else:
            raise ValueError(f"Algorithme inconnu: {algorithm_name}")

    def get_move(self, state: GameState) -> Optional[Tuple[int, Color, Color]]:
        """Retourne le meilleur coup selon l'algorithme"""
        return self.ai.get_move(state, self.player_id)


class HumanPlayer:
    """Représente un joueur humain"""

    def __init__(self, player_id: int):
        self.player_id = player_id

    def parse_move_input(self, move_str: str, state: GameState) -> Optional[Tuple[int, Color, Color]]:
        """
        Parse l'entrée utilisateur au format simple
        Formats acceptés:
        - "14B" : Trou 14, couleur BLEU
        - "3R" : Trou 3, couleur ROUGE
        - "5TR" : Trou 5, graines TRANSPARENTES comme ROUGE
        - "8TB" : Trou 8, graines TRANSPARENTES comme BLEU
        """
        move_str = move_str.strip().upper()

        # Parser le format: numero + couleur(s)
        if move_str.endswith("TR"):
            # Transparent comme Rouge
            try:
                hole = int(move_str[:-2])
                return (hole, Color.TRANSPARENT, Color.RED)
            except ValueError:
                return None

        elif move_str.endswith("TB"):
            # Transparent comme Bleu
            try:
                hole = int(move_str[:-2])
                return (hole, Color.TRANSPARENT, Color.BLUE)
            except ValueError:
                return None

        elif move_str.endswith("R"):
            # Rouge
            try:
                hole = int(move_str[:-1])
                return (hole, Color.RED, None)
            except ValueError:
                return None

        elif move_str.endswith("B"):
            # Bleu
            try:
                hole = int(move_str[:-1])
                return (hole, Color.BLUE, None)
            except ValueError:
                return None

        else:
            return None

    def is_move_valid(self, move: Tuple[int, Color, Color], state: GameState) -> bool:
        """Vérifie si un coup est valide"""
        hole, color, transparent_as = move

        # Vérifier que le trou est dans les trous du joueur
        if hole not in state.get_player_holes(self.player_id):
            return False

        # Vérifier que le trou a des graines de cette couleur
        if state.holes[hole][color] == 0:
            return False

        return True

    def get_move(self, state: GameState) -> Optional[Tuple[int, Color, Color]]:
        """Demande un coup au joueur avec le nouveau format"""
        # print(f"\n{'='*80}")
        # print(f"Au tour du Joueur {self.player_id}")
        # print(f"\nVos trous: {state.get_player_holes(self.player_id)}")
        # print(f"\nCoups valides (exemples):")
        # print(f"  14B  -> Trou 14, couleur BLEU")
        # print(f"  3R   -> Trou 3, couleur ROUGE")
        # print(f"  5TR  -> Trou 5, graines TRANSPARENTES comme ROUGE")
        # print(f"  8TB  -> Trou 8, graines TRANSPARENTES comme BLEU")

        # # Afficher les graines disponibles EN PARALLÈLE (horizontalement)
        # print(f"\n{'='*150}")
        # print(f"GRAINES DISPONIBLES - VUE PARALLÈLE:")
        # print(f"{'='*150}")

        opponent = 3 - self.player_id
        player_holes = state.get_player_holes(self.player_id)
        opponent_holes = state.get_player_holes(opponent)

        # # En-têtes
        # print(f"\n{'VOS TROUS (Joueur ' + str(self.player_id) + '):':^70} | {'TROUS ADVERSES (Joueur ' + str(opponent) + '):':^70}")
        # print("-" * 150)

        # # Afficher les trous en parallèle
        # for i in range(len(player_holes)):
        #     player_hole = player_holes[i]
        #     opponent_hole = opponent_holes[i]

        #     # Graines du joueur
        #     p_red = state.holes[player_hole][Color.RED]
        #     p_blue = state.holes[player_hole][Color.BLUE]
        #     p_trans = state.holes[player_hole][Color.TRANSPARENT]
        #     p_total = p_red + p_blue + p_trans
        #     player_str = f"Trou {player_hole:2d}: R={p_red} B={p_blue} T={p_trans}  (Total: {p_total})"

        #     # Graines de l'adversaire
        #     o_red = state.holes[opponent_hole][Color.RED]
        #     o_blue = state.holes[opponent_hole][Color.BLUE]
        #     o_trans = state.holes[opponent_hole][Color.TRANSPARENT]
        #     o_total = o_red + o_blue + o_trans
        #     opponent_str = f"Trou {opponent_hole:2d}: R={o_red} B={o_blue} T={o_trans}  (Total: {o_total})"

        #     print(f"{player_str:70} | {opponent_str:70}")

        # # Afficher les scores
        # print(f"\n{'='*150}")
        # print(f"SCORES ACTUELS:")
        # print(f"{'='*150}")
        # print(f"  Joueur {self.player_id} (VOUS): {state.captured_seeds[self.player_id]} graines capturées")
        # print(f"  Joueur {opponent} (ADVERSAIRE): {state.captured_seeds[opponent]} graines capturées")

        while True:
            try:
                move_input = input("\nEntrez votre coup (ex: 14B, 3R, 5TR, 8TB): ").strip()

                # Parser l'entrée
                move = self.parse_move_input(move_input, state)

                if move is None:
                    # print("Format invalide! Utilisez le format: [TROU][COULEUR]")
                    # print("Exemples: 14B, 3R, 5TR, 8TB")
                    continue

                # Vérifier la validité du coup
                if not self.is_move_valid(move, state):
                    hole, color, transparent_as = move
                    # print(f"Coup invalide! Le trou {hole} n'a pas de graines {color.value}")
                    continue

                return move

            except Exception as e:
                # print(f"Erreur: {e}")
                # print("Veuillez réessayer!")
                continue


class GameManager:
    """Gère une partie complète"""

    def __init__(self, player1_type: str, player1_config: dict,
                 player2_type: str, player2_config: dict):
        """
        player_type: "human" ou nom de l'algorithme ("bfs", "dfs", "minimax", "iddfs")
        player_config: dict avec "depth" pour les IA
        """
        self.state = GameState()
        self.engine = GameEngine(self.state)

        # Création des joueurs
        self.players = {}
        self._create_player(1, player1_type, player1_config)
        self._create_player(2, player2_type, player2_config)

        self.move_history = []
        self.total_moves = 0

    def _create_player(self, player_id: int, player_type: str, config: dict):
        """Crée un joueur selon le type"""
        if player_type.lower() == "human":
            self.players[player_id] = HumanPlayer(player_id)
        else:
            depth = config.get("depth", 3)
            self.players[player_id] = AIPlayer(player_id, player_type, depth)

    def play_turn(self, verbose: bool = True) -> bool:
        """
        Exécute un tour pour le joueur courant
        Retourne True si le coup a été joué, False si le jeu est terminé
        """
        if self.state.is_game_over():
            return False

        player_id = self.state.current_player
        player = self.players[player_id]

        # Affiche le plateau de manière claire
        if verbose:
            self._print_board()

        # Obtient le coup
        start_time = time.time()
        move = player.get_move(self.state)
        elapsed = time.time() - start_time

        # Vérifier si le joueur a dépassé le temps limite
        if elapsed > MAX_TIME_PER_MOVE:
            if verbose:
                # print(f"\n❌ TIMEOUT! Joueur {player_id} a dépassé le temps limite!")
                # print(f"   Temps utilisé: {elapsed:.3f}s (Limite: {MAX_TIME_PER_MOVE}s)")
                # print(f"   Joueur {player_id} PERD par timeout!")
                pass

            # Forcer la fin du jeu avec une défaite pour le joueur qui a timeout
            opponent = 3 - player_id
            self.state.captured_seeds[opponent] = 49  # L'adversaire gagne automatiquement
            self.state.game_over = True
            self.state.winner = opponent

            self.move_history.append({
                "player": player_id,
                "hole": None,
                "color": None,
                "transparent_as": None,
                "time": elapsed,
                "timeout": True
            })

            return False

        if move is None:
            # print(f"Aucun coup valide pour le joueur {player_id}")
            return False

        hole, color, transparent_as = move

        if not self.engine.play_move(hole, color, transparent_as):
            # print(f"Coup invalide: {hole}{color.value}")
            return False

        self.move_history.append({
            "player": player_id,
            "hole": hole,
            "color": color.value,
            "transparent_as": transparent_as.value if transparent_as else None,
            "time": elapsed
        })
        self.total_moves += 1

        if verbose:
            if transparent_as:
                print(f"{hole}T{transparent_as.value}")
            else:
                print(f"{hole}{color.value}")
            # print(f"   Temps de calcul: {elapsed:.3f}s")

        return True

    def _print_board(self):
        """Affiche le plateau de manière claire et formatée"""
        # print("\n" + "="*100)
        # print(f"Score P1: {self.state.captured_seeds[1]:2d} | Score P2: {self.state.captured_seeds[2]:2d}")

        # # Ligne 1 : Trous 1-8
        # print("\n| ", end="")
        # for hole in range(1, 9):
        #     print(f"Field:{hole}| ", end="")
        # print()

        # # Ligne 2 : Graines ROUGES (trous 1-8)
        # print("| ", end="")
        # for hole in range(1, 9):
        #     red_count = self.state.holes[hole][Color.RED]
        #     print(f"RED:{red_count}  | ", end="")
        # print()

        # # Ligne 3 : Graines BLEUES (trous 1-8)
        # print("| ", end="")
        # for hole in range(1, 9):
        #     blue_count = self.state.holes[hole][Color.BLUE]
        #     print(f"BLUE:{blue_count} | ", end="")
        # print()

        # # Ligne 4 : Graines TRANSPARENTES (trous 1-8)
        # print("| ", end="")
        # for hole in range(1, 9):
        #     trans_count = self.state.holes[hole][Color.TRANSPARENT]
        #     print(f"TRSP:{trans_count} | ", end="")
        # print()

        # # Ligne vide
        # print()

        # # Ligne 5 : Trous 9-16
        # print("| ", end="")
        # for hole in range(9, 17):
        #     print(f"Field:{hole}| ", end="")
        # print()

        # # Ligne 6 : Graines ROUGES (trous 9-16)
        # print("| ", end="")
        # for hole in range(9, 17):
        #     red_count = self.state.holes[hole][Color.RED]
        #     print(f"RED:{red_count}  | ", end="")
        # print()

        # # Ligne 7 : Graines BLEUES (trous 9-16)
        # print("| ", end="")
        # for hole in range(9, 17):
        #     blue_count = self.state.holes[hole][Color.BLUE]
        #     print(f"BLUE:{blue_count} | ", end="")
        # print()

        # # Ligne 8 : Graines TRANSPARENTES (trous 9-16)
        # print("| ", end="")
        # for hole in range(9, 17):
        #     trans_count = self.state.holes[hole][Color.TRANSPARENT]
        #     print(f"TRSP:{trans_count} | ", end="")
        # print()

        # print("\n" + "="*100)
        pass

    def play_game(self, verbose: bool = True, max_turns: int = 400) -> int:
        """
        Joue une partie complète
        Retourne l'ID du gagnant (0 pour égalité)
        """
        turn_count = 0

        while not self.state.is_game_over() and turn_count < max_turns:
            if not self.play_turn(verbose):
                break
            turn_count += 1

        # if verbose:
        #     print("\n" + "="*80)
        #     print("FIN DE LA PARTIE")
        #     print("="*80)
        #     print(f"Nombre de coups joués: {self.state.move_count}")

        #     # Vérifier si la partie s'est terminée par timeout
        #     if self.move_history and self.move_history[-1].get("timeout", False):
        #         timeout_player = self.move_history[-1]["player"]
        #         print(f"Raison: Joueur {timeout_player} a dépassé le temps limite ({MAX_TIME_PER_MOVE}s)!")
        #     # Afficher la raison de fin de partie
        #     elif self.state.move_count >= self.state.MAX_MOVES:
        #         print(f"Raison: Limite de {self.state.MAX_MOVES} coups atteinte!")
        #     elif self.state.captured_seeds[1] >= 49:
        #         print("Raison: Joueur 1 a capturé 49+ graines!")
        #     elif self.state.captured_seeds[2] >= 49:
        #         print("Raison: Joueur 2 a capturé 49+ graines!")
        #     elif self.state.captured_seeds[1] >= 40 and self.state.captured_seeds[2] >= 40:
        #         print("Raison: Les deux joueurs ont 40+ graines!")
        #     elif self.state.get_seeds_on_board() < 10:
        #         print("Raison: Moins de 10 graines sur le plateau!")

        #     print(self.state)

        #     winner = self.state.get_winner()
        #     if winner == 0:
        #         print("ÉGALITÉ!")
        #     else:
        #         print(f"JOUEUR {winner} GAGNE!")

        return self.state.get_winner()

    def get_game_stats(self) -> dict:
        """Retourne les statistiques de la partie"""
        return {
            "total_moves": self.total_moves,
            "player1_captured": self.state.captured_seeds[1],
            "player2_captured": self.state.captured_seeds[2],
            "seeds_on_board": self.state.get_seeds_on_board(),
            "move_history": self.move_history
        }


class Tournament:
    """Gère un tournoi entre plusieurs IA"""

    def __init__(self):
        self.results = []

    def play_match(self, player1_type: str, player1_config: dict,
                   player2_type: str, player2_config: dict,
                   verbose: bool = False) -> dict:
        """Joue un match entre deux joueurs"""
        manager = GameManager(player1_type, player1_config,
                             player2_type, player2_config)
        winner = manager.play_game(verbose)

        stats = manager.get_game_stats()
        match_result = {
            "player1": player1_type,
            "player1_config": player1_config,
            "player2": player2_type,
            "player2_config": player2_config,
            "winner": winner,
            "stats": stats
        }

        self.results.append(match_result)
        return match_result

    def print_results(self):
        """Affiche les résultats du tournoi"""
        # print("\n" + "="*80)
        # print("RÉSULTATS DU TOURNOI")
        # print("="*80)

        # for i, result in enumerate(self.results, 1):
        #     print(f"\nMatch {i}:")
        #     print(f"  {result['player1']} vs {result['player2']}")

        #     if result['winner'] == 0:
        #         print(f"  Résultat: ÉGALITÉ")
        #     else:
        #         print(f"  Gagnant: Joueur {result['winner']} ({result['player1'] if result['winner'] == 1 else result['player2']})")

        #     print(f"  Graines capturées - J1: {result['stats']['player1_captured']}, J2: {result['stats']['player2_captured']}")
        #     print(f"  Total des coups: {result['stats']['total_moves']}")
        pass
