"""
Interface interactive - Menu principal pour jouer
"""

from game_manager import GameManager, Tournament, HumanPlayer, AIPlayer
from game_rules import Color, GameState

def print_game_board(state: GameState):
    """
    Affiche le plateau du jeu avec toutes les informations
    Format similaire à l'image fournie - CLAIR ET LISIBLE
    """
    # print("\n")

    # # Affichage des scores
    # print(f"Score P1: {state.captured_seeds[1]:2d} | Score P2: {state.captured_seeds[2]:2d}")

    # # Ligne 1 : Trous 1-8
    # print("| ", end="")
    # for hole in range(1, 9):
    #     print(f"Field:{hole}| ", end="")
    # print()

    # # Ligne 2 : Graines ROUGES (trous 1-8)
    # print("| ", end="")
    # for hole in range(1, 9):
    #     red_count = state.holes[hole][Color.RED]
    #     print(f"RED:{red_count}  | ", end="")
    # print()

    # # Ligne 3 : Graines BLEUES (trous 1-8)
    # print("| ", end="")
    # for hole in range(1, 9):
    #     blue_count = state.holes[hole][Color.BLUE]
    #     print(f"BLUE:{blue_count} | ", end="")
    # print()

    # # Ligne 4 : Graines TRANSPARENTES (trous 1-8)
    # print("| ", end="")
    # for hole in range(1, 9):
    #     trans_count = state.holes[hole][Color.TRANSPARENT]
    #     print(f"TRSP:{trans_count} | ", end="")
    # print()

    # # Ligne vide
    # print()

    # # Ligne 5 : Trous 9-16
    # print("| ", end="")
    # for hole in range(9, 17):
    #     print(f"Field:{hole}| ", end="")
    #     print()

    # # Ligne 6 : Graines ROUGES (trous 9-16)
    # print("| ", end="")
    # for hole in range(9, 17):
    #     red_count = state.holes[hole][Color.RED]
    #     print(f"RED:{red_count}  | ", end="")
    # print()

    # # Ligne 7 : Graines BLEUES (trous 9-16)
    # print("| ", end="")
    # for hole in range(9, 17):
    #     blue_count = state.holes[hole][Color.BLUE]
    #     print(f"BLUE:{blue_count} | ", end="")
    # print()

    # # Ligne 8 : Graines TRANSPARENTES (trous 9-16)
    # print("| ", end="")
    # for hole in range(9, 17):
    #     trans_count = state.holes[hole][Color.TRANSPARENT]
    #     print(f"TRSP:{trans_count} | ", end="")
    # print()

    # print()
    pass

def print_menu():
    """Affiche le menu principal"""
    print("\n" + "="*80)
    print("JEU MANCALA À 16 TROUS - 4 IA AVEC ALGORITHMES DE RECHERCHE")
    print("="*80)
    print("\n1. Jouer contre l'IA")
    print("2. IA vs IA")
    print("3. Tournoi entre IA")
    print("4. Quitter")
    print("\nChoisissez une option (1-4):")

def print_ai_menu():
    """Affiche le menu des IA"""
    print("\nSélectionnez l'IA:")
    print("1. BFS (Breadth-First Search) - Profondeur 2")
    print("2. DFS (Depth-First Search) - Profondeur 3")
    print("3. Min-Max - Profondeur 4")
    print("4. Alpha-Beta Pruning - Profondeur 5 (Recommandé)")
    print("5. Iterative Deepening DFS - Profondeur 6")

def get_ai_choice(prompt: str = "Choisissez une IA (1-5):") -> tuple:
    """Demande à l'utilisateur de choisir une IA"""
    print_ai_menu()

    while True:
        try:
            choice = int(input(prompt))

            if choice == 1:
                return "bfs", {"depth": 2}
            elif choice == 2:
                return "dfs", {"depth": 3}
            elif choice == 3:
                return "minimax", {"depth": 4}
            elif choice == 4:
                return "alphabeta", {"depth": 5}
            elif choice == 5:
                return "iddfs", {"depth": 6}
            else:
                print("Choix invalide! Entrez 1, 2, 3, 4 ou 5.")
        except ValueError:
            print("Veuillez entrer un nombre!")

def play_human_vs_ai():
    """Lance une partie Humain vs IA"""
    print("\n" + "="*80)
    print("HUMAIN VS IA")
    print("="*80)

    print("\nQuel joueur êtes-vous?")
    print("1. Joueur 1 (trous impairs: 1,3,5,7,9,11,13,15)")
    print("2. Joueur 2 (trous pairs: 2,4,6,8,10,12,14,16)")

    while True:
        try:
            choice = int(input("Choix (1 ou 2): "))
            if choice in [1, 2]:
                human_player = choice
                break
            else:
                print("Choix invalide!")
        except ValueError:
            print("Veuillez entrer un nombre!")

    ai_player = 3 - human_player

    print(f"\nVous êtes le Joueur {human_player}")
    print(f"L'IA sera le Joueur {ai_player}")

    ai_type, ai_config = get_ai_choice("Choisissez l'IA pour le Joueur " + str(ai_player) + " (1-4): ")

    if human_player == 1:
        manager = GameManager("human", {}, ai_type, ai_config)
    else:
        manager = GameManager(ai_type, ai_config, "human", {})

    print("\n" + "="*80)
    print("DÉBUT DE LA PARTIE - Seuls les coups seront affichés")
    print("="*80)

    # Pas d'affichage du plateau initial
    # print_game_board(manager.state)

    manager.play_game(verbose=True)

    # Affichage du résultat
    stats = manager.get_game_stats()
    winner = manager.state.get_winner()

    print("\n" + "="*80)
    if winner == 0:
        print(f"RÉSULTAT: ÉGALITÉ ({stats['player1_captured']} - {stats['player2_captured']})")
    elif winner == human_player:
        print(f"RÉSULTAT: VOUS AVEZ GAGNÉ! ({stats['player1_captured']} - {stats['player2_captured']})")
    else:
        print(f"RÉSULTAT: L'IA A GAGNÉ! ({stats['player1_captured']} - {stats['player2_captured']})")
    print("="*80)

def play_ai_vs_ai():
    """Lance une partie IA vs IA"""
    print("\n" + "="*80)
    print("IA VS IA")
    print("="*80)

    print("\nSélectionnez l'IA pour le Joueur 1 (trous impairs):")
    ai1_type, ai1_config = get_ai_choice()

    print("\nSélectionnez l'IA pour le Joueur 2 (trous pairs):")
    ai2_type, ai2_config = get_ai_choice()

    manager = GameManager(ai1_type, ai1_config, ai2_type, ai2_config)

    print("\n" + "="*80)
    print(f"DÉBUT: {ai1_type.upper()} (J1) vs {ai2_type.upper()} (J2)")
    print("Seuls les coups seront affichés")
    print("="*80)

    # Pas d'affichage du plateau initial
    # print_game_board(manager.state)

    manager.play_game(verbose=True)

    # Affichage du résultat
    stats = manager.get_game_stats()
    winner = manager.state.get_winner()

    print("\n" + "="*80)
    if winner == 0:
        print(f"RÉSULTAT: ÉGALITÉ ({stats['player1_captured']} - {stats['player2_captured']})")
    elif winner == 1:
        print(f"RÉSULTAT: {ai1_type.upper()} (J1) A GAGNÉ! ({stats['player1_captured']} - {stats['player2_captured']})")
    else:
        print(f"RÉSULTAT: {ai2_type.upper()} (J2) A GAGNÉ! ({stats['player1_captured']} - {stats['player2_captured']})")
    print("="*80)

def play_tournament():
    """Lance un tournoi entre les 5 IA"""
    # print("\n" + "="*80)
    # print("TOURNOI - TOUS LES IA SE BATTENT")
    # print("="*80)

    tournament = Tournament()

    ais = [
        ("bfs", {"depth": 2}),
        ("dfs", {"depth": 3}),
        ("minimax", {"depth": 4}),
        ("alphabeta", {"depth": 5}),
        ("iddfs", {"depth": 6})
    ]

    ai_names = ["BFS", "DFS", "Min-Max", "Alpha-Beta", "ID-DFS"]

    # print("\nOuverture du tournoi...")

    match_count = 0
    for i, (ai1_type, ai1_config) in enumerate(ais):
        for j, (ai2_type, ai2_config) in enumerate(ais):
            if i < j:  # Évite les doublons
                match_count += 1
                # print(f"\nMatch {match_count}: {ai_names[i]} vs {ai_names[j]}")

                manager = GameManager(ai1_type, ai1_config, ai2_type, ai2_config)
                manager.play_game(verbose=False)

                stats = manager.get_game_stats()
                winner = manager.state.get_winner()

                if winner == 0:
                    print(f"  Résultat: ÉGALITÉ ({stats['player1_captured']} - {stats['player2_captured']})")
                else:
                    winner_name = ai_names[i] if winner == 1 else ai_names[j]
                    print(f"  Gagnant: {winner_name} ({stats['player1_captured']} - {stats['player2_captured']})")

    tournament.print_results()

def main():
    """Boucle principale"""
    while True:
        print_menu()

        try:
            choice = input().strip()

            if choice == "1":
                play_human_vs_ai()
            elif choice == "2":
                play_ai_vs_ai()
            elif choice == "3":
                play_tournament()
            elif choice == "4":
                print("\nAu revoir!")
                break
            else:
                print("Choix invalide! Entrez 1, 2, 3 ou 4.")

        except KeyboardInterrupt:
            print("\n\nPartie annulée par l'utilisateur.")
            break
        except Exception as e:
            print(f"\nErreur: {e}")
            print("Veuillez réessayer.")

if __name__ == "__main__":
    main()
