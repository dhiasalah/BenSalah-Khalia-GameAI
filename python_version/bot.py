"""
Bot pour la plateforme awale-arena
Format compatible:
- Entrée: mouvements au format "NX" ou "NTX" (N=numéro trou, X=R/B/T, T=transparent)
- Sortie: coup au même format
- Utilise MinMax AI de ai_algorithms.py
"""

import sys
import random
from typing import List, Optional, Tuple
from game_rules import GameState, Color
from game_engine import GameEngine, MoveGenerator
from ai_algorithms import MinMaxBot


def main():
    """
    Fonction principale pour interfacer avec awale-arena
    Lire depuis stdin, traiter les mouvements, envoyer les réponses
    """
    # Récupérer le numéro du joueur (1 ou 2)
    my_player = int(sys.argv[1])  # 1 ou 2

    # Initialiser le jeu et le bot avec MinMax AI
    state = GameState()
    engine = GameEngine(state)
    bot = MinMaxBot(depth=4)  # MinMax avec profondeur 4

    # Boucle principale
    for line in sys.stdin:
        move_str = line.strip()

        # Traiter les signaux spéciaux
        if move_str == "END":
            break

        # Pour Player 2, attendre le premier coup de Player 1
        if move_str == "START" and my_player == 2:
            continue  # Attendre le prochain message

        if move_str != "START":
            # Appliquer le coup de l'adversaire
            hole, color, trans_as = parse_move(move_str)
            if hole and color:
                engine.play_move(hole, color, trans_as)

        # S'assurer que le joueur courant est correct
        state.current_player = my_player
        
        # Obtenir le meilleur coup avec MinMax
        best_move = bot.get_move(state, my_player)

        if best_move:
            hole, color, trans_as = best_move
            my_move = format_move(hole, color, trans_as)
            
            # Appliquer notre coup
            engine.play_move(hole, color, trans_as)
        else:
            # Pas de coup disponible (ne devrait pas arriver)
            my_move = "NOMOVE"

        # Envoyer le coup à la plateforme
        sys.stdout.write(my_move + "\n")
        sys.stdout.flush()


def parse_move(move_str: str) -> Tuple[Optional[int], Optional[Color], Optional[Color]]:
    """
    Parse un mouvement au format "NX" ou "NTX"
    Retourne (hole, color, transparent_as)
    """
    try:
        move_str = move_str.strip().upper()
        
        if 'T' in move_str[1:-1]:  # Transparent au milieu (ex: "3TR", "3TB")
            hole = int(move_str[:-2])
            trans_as_str = move_str[-1]
            color = Color.TRANSPARENT
            trans_as = Color.RED if trans_as_str == 'R' else Color.BLUE
            return (hole, color, trans_as)
        else:  # Normal (ex: "3R", "4B")
            hole = int(move_str[:-1])
            color_str = move_str[-1]
            color = Color.RED if color_str == 'R' else Color.BLUE
            return (hole, color, None)
    except:
        return (None, None, None)


def format_move(hole: int, color: Color, trans_as: Optional[Color]) -> str:
    """
    Formate un mouvement pour l'envoi
    """
    if color == Color.TRANSPARENT:
        return f"{hole}T{trans_as.value}"
    else:
        return f"{hole}{color.value}"


if __name__ == "__main__":
    main()
