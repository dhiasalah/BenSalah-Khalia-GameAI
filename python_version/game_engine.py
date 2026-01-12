"""
Moteur de jeu Mancala - Version CORRIGÉE avec règles complètes de capture
Les graines transparentes sont distribuées AVANT les autres graines de la couleur désignée
"""

from game_rules import GameState, Color
from typing import List, Tuple, Set

class GameEngine:
    """Moteur de jeu qui gère l'exécution des coups"""

    def __init__(self, game_state: GameState):
        self.state = game_state
        self.debug = False  # Mettre à True pour voir le détail des captures

    def next_hole(self, current_hole: int) -> int:
        """Retourne le trou suivant en sens horaire"""
        if current_hole == 16:
            return 1
        else:
            return current_hole + 1

    def prev_hole(self, current_hole: int) -> int:
        """Retourne le trou précédent (sens anti-horaire)"""
        if current_hole == 1:
            return 16
        else:
            return current_hole - 1

    def is_opponent_hole(self, hole: int, player: int) -> bool:
        """Vérifie si un trou appartient à l'adversaire"""
        player_holes = self.state.get_player_holes(player)
        return hole not in player_holes

    def play_move(self, hole: int, color: Color, transparent_as: Color = None) -> bool:
        """
        Exécute un coup avec les règles complètes :
        - Si les graines sont rouges : distribuées dans tous les trous
        - Si les graines sont bleues : distribuées uniquement dans les trous adverses
        - Si transparentes : jouées COMME la couleur désignée (suivent ses règles de distribution)
          MAIS restent transparentes sur le plateau

        IMPORTANT: Les graines transparentes ne sont jouées QUE si color == Color.TRANSPARENT
        Quand on demande "1B", SEULES les graines BLEUES sont jouées, pas les transparentes.

        hole: numéro du trou
        color: couleur sélectionnée (RED, BLUE, ou TRANSPARENT)
        transparent_as: Red ou Blue si la couleur est Transparent (détermine les règles de distribution)
        """
        player = self.state.current_player

        # Vérification de validité
        if self.state.holes[hole][color] == 0:
            return False

        if hole not in self.state.get_player_holes(player):
            return False

        if color == Color.TRANSPARENT and transparent_as is None:
            return False

        # Déterminer les graines à distribuer
        if color == Color.TRANSPARENT:
            # Les graines transparentes + les graines de la couleur désignée sont distribuées ensemble
            # Les transparentes RESTENT transparentes sur le plateau
            trans_seeds = self.state.holes[hole][Color.TRANSPARENT]
            color_seeds = self.state.holes[hole][transparent_as]

            distribution_rule = transparent_as  # Règle de distribution (RED ou BLUE)

            # Distribuer d'abord les graines transparentes, puis les graines de couleur
            seeds_transparent = trans_seeds
            seeds_colored = color_seeds

            self.state.holes[hole][Color.TRANSPARENT] = 0
            self.state.holes[hole][transparent_as] = 0

            # On doit distribuer les deux types de graines
            seeds_to_distribute = seeds_transparent + seeds_colored
            seed_color = None  # Sera géré spécialement

            if self.debug:
                print(f"\n[DEBUG] Joueur {player} joue trou {hole}, TRANSPARENT comme {transparent_as.value}")
                print(f"[DEBUG] Graines transparentes: {seeds_transparent}, Graines {transparent_as.value}: {seeds_colored}")
        else:
            # Couleur normale (RED ou BLUE) - SEULES ces graines sont jouées
            seeds_to_distribute = self.state.holes[hole][color]
            distribution_rule = color  # Règle de distribution
            seed_color = color  # Couleur à placer
            seeds_transparent = 0
            seeds_colored = seeds_to_distribute
            self.state.holes[hole][color] = 0

        current_hole = hole
        last_hole_seeded = None

        if self.debug and seed_color is not None:
            print(f"\n[DEBUG] Joueur {player} joue trou {hole}, couleur {seed_color.value}")
            print(f"[DEBUG] Distribution selon les règles: {distribution_rule.value}")
            print(f"[DEBUG] Graines à distribuer: {seeds_to_distribute}")

        # Distribuer les graines selon les règles de distribution_rule
        # Si color == TRANSPARENT, on distribue d'abord les transparentes, puis les colorées

        if color == Color.TRANSPARENT:
            # Distribution spéciale: transparentes d'abord, puis colorées
            trans_remaining = seeds_transparent
            colored_remaining = seeds_colored

            while trans_remaining > 0 or colored_remaining > 0:
                current_hole = self.next_hole(current_hole)

                # Déterminer quelle graine distribuer (transparente d'abord)
                if trans_remaining > 0:
                    current_seed_color = Color.TRANSPARENT
                    trans_remaining -= 1
                else:
                    current_seed_color = transparent_as
                    colored_remaining -= 1

                # Distribution selon les règles
                if distribution_rule == Color.RED:
                    # Les rouges vont dans tous les trous
                    self.state.holes[current_hole][current_seed_color] += 1
                    last_hole_seeded = current_hole
                    if self.debug:
                        print(f"[DEBUG] Graine {current_seed_color.value} -> Trou {current_hole} (règle RED)")

                elif distribution_rule == Color.BLUE:
                    # Les bleus vont uniquement dans les trous adverses
                    if self.is_opponent_hole(current_hole, player):
                        self.state.holes[current_hole][current_seed_color] += 1
                        last_hole_seeded = current_hole
                        if self.debug:
                            print(f"[DEBUG] Graine {current_seed_color.value} -> Trou adversaire {current_hole} (règle BLUE)")
                    else:
                        # Si ce n'est pas un trou adverse, on remet la graine et on continue
                        if trans_remaining >= 0 and current_seed_color == Color.TRANSPARENT:
                            trans_remaining += 1
                        else:
                            colored_remaining += 1
        else:
            # Distribution normale pour RED ou BLUE seul
            seeds_remaining = seeds_to_distribute

            while seeds_remaining > 0:
                current_hole = self.next_hole(current_hole)

                # Distribution selon les règles de distribution_rule
                if distribution_rule == Color.RED:
                    # Les rouges vont dans tous les trous
                    self.state.holes[current_hole][seed_color] += 1
                    last_hole_seeded = current_hole
                    if self.debug:
                        print(f"[DEBUG] Graine {seed_color.value} -> Trou {current_hole} (règle RED)")
                    seeds_remaining -= 1

                elif distribution_rule == Color.BLUE:
                    # Les bleus vont uniquement dans les trous adverses
                    if self.is_opponent_hole(current_hole, player):
                        self.state.holes[current_hole][seed_color] += 1
                        last_hole_seeded = current_hole
                        if self.debug:
                            print(f"[DEBUG] Graine {seed_color.value} -> Trou adversaire {current_hole} (règle BLUE)")
                        seeds_remaining -= 1

        # Capture à partir du dernier trou semé (si une graine a été semée)
        if last_hole_seeded is not None:
            self._capture_seeds(last_hole_seeded, player)

        # Incrémenter le compteur de coups
        self.state.move_count += 1

        # Changement de joueur
        self.state.current_player = 3 - player  # Alterne entre 1 et 2

        return True

    def _capture_seeds(self, last_hole: int, player: int):
        """
        Gère la capture des graines
        Règles :
        - La capture peut se faire depuis N'IMPORTE quel trou (y compris ses propres trous)
        - Règle officielle: "it is allowed to take the seeds from its own hole"
        - Remonte en arrière en sens anti-horaire
        - Capture les trous avec exactement 2 ou 3 graines (toutes couleurs confondues)
        - S'arrête dès qu'on rencontre un trou qui n'a pas 2-3 graines
        """
        if self.debug:
            print(f"[DEBUG] Vérification de capture en partant du trou {last_hole}")

        current_hole = last_hole
        captured_total = 0

        # Remonte en arrière en sens anti-horaire depuis n'importe quel trou
        while True:
            total_seeds = self.state.get_total_seeds(current_hole)

            if self.debug:
                print(f"[DEBUG] Trou {current_hole}: {total_seeds} graines")

            if total_seeds == 2 or total_seeds == 3:
                # Capture ce trou
                seeds_captured = total_seeds
                captured_total += seeds_captured

                if self.debug:
                    print(f"[DEBUG] ✓ Capture du trou {current_hole} ({seeds_captured} graines)")

                # Réinitialise le trou
                for col in Color:
                    self.state.holes[current_hole][col] = 0

                self.state.captured_seeds[player] += seeds_captured

                # Continue vers le trou précédent
                current_hole = self.prev_hole(current_hole)
            else:
                # Arrête la capture
                if self.debug:
                    print(f"[DEBUG] ✗ Fin de capture - le trou {current_hole} n'a pas 2-3 graines ({total_seeds})")
                break

        if self.debug and captured_total > 0:
            print(f"[DEBUG] Total capturé: {captured_total} graines")

    def get_board_state_string(self) -> str:
        """Retourne une représentation lisible du plateau"""
        return str(self.state)


# Pre-computed player holes for speed
_P1_HOLES = (1, 3, 5, 7, 9, 11, 13, 15)
_P2_HOLES = (2, 4, 6, 8, 10, 12, 14, 16)


class MoveGenerator:
    """Générateur de mouvements pour l'IA - Optimized"""

    @staticmethod
    def get_all_moves(state: GameState, player: int) -> List[Tuple[int, Color, Color]]:
        """
        Génère tous les coups possibles pour un joueur - Optimized
        Inclut les graines RED, BLUE et TRANSPARENT (comme transparentRED et transparentBLUE)
        """
        moves = []
        holes = state.holes
        player_holes = _P1_HOLES if player == 1 else _P2_HOLES

        for hole in player_holes:
            h = holes[hole]
            # Générer les coups pour les couleurs ROUGE et BLEU
            if h[Color.RED] > 0:
                moves.append((hole, Color.RED, None))
            if h[Color.BLUE] > 0:
                moves.append((hole, Color.BLUE, None))
            # Générer les coups pour TRANSPARENT (comme RED ou comme BLUE)
            if h[Color.TRANSPARENT] > 0:
                moves.append((hole, Color.TRANSPARENT, Color.RED))
                moves.append((hole, Color.TRANSPARENT, Color.BLUE))

        return moves

    @staticmethod
    def apply_move(state: GameState, hole: int, color: Color, transparent_as: Color = None) -> GameState:
        """
        Applique un coup à un état et retourne le nouvel état - Optimized
        """
        new_state = state.copy()
        engine = GameEngine(new_state)
        engine.play_move(hole, color, transparent_as)
        return new_state
