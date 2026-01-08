/**
 * Interface interactive - Menu principal pour jouer
 */

#include "game_manager.h"
#include "game_rules.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

void printGameBoard(const GameState& state) {
    /**
    Affiche le plateau du jeu avec toutes les informations
    Format similaire à l'image fournie - CLAIR ET LISIBLE
    */
    // std::cout << "\n" << std::endl;

    // // Affichage des scores
    // std::cout << "Score P1: " << state.captured_seeds.at(1) << " | Score P2: " << state.captured_seeds.at(2) << std::endl;

    // // Ligne 1 : Trous 1-8
    // std::cout << "| ";
    // for (int hole = 1; hole <= 8; hole++) {
    //     std::cout << "Field:" << hole << "| ";
    // }
    // std::cout << std::endl;

    // // Ligne 2 : Graines ROUGES (trous 1-8)
    // std::cout << "| ";
    // for (int hole = 1; hole <= 8; hole++) {
    //     int red_count = state.holes.at(hole).at(Color::RED);
    //     std::cout << "RED:" << red_count << "  | ";
    // }
    // std::cout << std::endl;

    // // Ligne 3 : Graines BLEUES (trous 1-8)
    // std::cout << "| ";
    // for (int hole = 1; hole <= 8; hole++) {
    //     int blue_count = state.holes.at(hole).at(Color::BLUE);
    //     std::cout << "BLUE:" << blue_count << " | ";
    // }
    // std::cout << std::endl;

    // // Ligne 4 : Graines TRANSPARENTES (trous 1-8)
    // std::cout << "| ";
    // for (int hole = 1; hole <= 8; hole++) {
    //     int trans_count = state.holes.at(hole).at(Color::TRANSPARENT);
    //     std::cout << "TRSP:" << trans_count << " | ";
    // }
    // std::cout << std::endl;

    // // Ligne vide
    // std::cout << std::endl;

    // // Ligne 5 : Trous 9-16
    // std::cout << "| ";
    // for (int hole = 9; hole <= 16; hole++) {
    //     std::cout << "Field:" << hole << "| ";
    // }
    // std::cout << std::endl;

    // // Ligne 6 : Graines ROUGES (trous 9-16)
    // std::cout << "| ";
    // for (int hole = 9; hole <= 16; hole++) {
    //     int red_count = state.holes.at(hole).at(Color::RED);
    //     std::cout << "RED:" << red_count << "  | ";
    // }
    // std::cout << std::endl;

    // // Ligne 7 : Graines BLEUES (trous 9-16)
    // std::cout << "| ";
    // for (int hole = 9; hole <= 16; hole++) {
    //     int blue_count = state.holes.at(hole).at(Color::BLUE);
    //     std::cout << "BLUE:" << blue_count << " | ";
    // }
    // std::cout << std::endl;

    // // Ligne 8 : Graines TRANSPARENTES (trous 9-16)
    // std::cout << "| ";
    // for (int hole = 9; hole <= 16; hole++) {
    //     int trans_count = state.holes.at(hole).at(Color::TRANSPARENT);
    //     std::cout << "TRSP:" << trans_count << " | ";
    // }
    // std::cout << std::endl;

    // std::cout << std::endl;
}

void printMenu() {
    /**Affiche le menu principal*/
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "JEU MANCALA À 16 TROUS - 4 IA AVEC ALGORITHMES DE RECHERCHE" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "\n1. Jouer contre l'IA" << std::endl;
    std::cout << "2. IA vs IA" << std::endl;
    std::cout << "3. Tournoi entre IA" << std::endl;
    std::cout << "4. Quitter" << std::endl;
    std::cout << "\nChoisissez une option (1-4): " << std::flush;
}

void printAIMenu() {
    /**Affiche le menu des IA*/
    std::cout << "\nSélectionnez l'IA:" << std::endl;
    std::cout << "1. BFS (Breadth-First Search) - Profondeur 2" << std::endl;
    std::cout << "2. DFS (Depth-First Search) - Profondeur 3" << std::endl;
    std::cout << "3. Min-Max - Profondeur 4" << std::endl;
    std::cout << "4. Alpha-Beta Pruning - Profondeur 5 (Recommandé)" << std::endl;
    std::cout << "5. Iterative Deepening DFS - Profondeur 6" << std::endl;
}

std::pair<std::string, std::map<std::string, int>> getAIChoice(const std::string& prompt = "Choisissez une IA (1-5): ") {
    /**Demande à l'utilisateur de choisir une IA*/
    printAIMenu();

    while (true) {
        try {
            std::cout << prompt << std::flush;
            std::string input;
            std::getline(std::cin, input);
            int choice = std::stoi(input);

            if (choice == 1) {
                return {"bfs", {{"depth", 2}}};
            } else if (choice == 2) {
                return {"dfs", {{"depth", 3}}};
            } else if (choice == 3) {
                return {"minimax", {{"depth", 4}}};
            } else if (choice == 4) {
                return {"alphabeta", {{"depth", 5}}};
            } else if (choice == 5) {
                return {"iddfs", {{"depth", 6}}};
            } else {
                std::cout << "Choix invalide! Entrez 1, 2, 3, 4 ou 5." << std::endl;
            }
        } catch (...) {
            std::cout << "Veuillez entrer un nombre!" << std::endl;
        }
    }
}

void playHumanVsAI() {
    /**Lance une partie Humain vs IA*/
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "HUMAIN VS IA" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

    std::cout << "\nQuel joueur êtes-vous?" << std::endl;
    std::cout << "1. Joueur 1 (trous impairs: 1,3,5,7,9,11,13,15)" << std::endl;
    std::cout << "2. Joueur 2 (trous pairs: 2,4,6,8,10,12,14,16)" << std::endl;

    int human_player = 0;
    while (true) {
        try {
            std::cout << "Choix (1 ou 2): ";
            std::string input;
            std::getline(std::cin, input);
            int choice = std::stoi(input);
            if (choice == 1 || choice == 2) {
                human_player = choice;
                break;
            } else {
                std::cout << "Choix invalide!" << std::endl;
            }
        } catch (...) {
            std::cout << "Veuillez entrer un nombre!" << std::endl;
        }
    }

    int ai_player = 3 - human_player;

    std::cout << "\nVous êtes le Joueur " << human_player << std::endl;
    std::cout << "L'IA sera le Joueur " << ai_player << std::endl;

    auto [ai_type, ai_config] = getAIChoice("Choisissez l'IA pour le Joueur " + std::to_string(ai_player) + " (1-4): ");

    std::unique_ptr<GameManager> manager;
    if (human_player == 1) {
        manager = std::make_unique<GameManager>("human", std::map<std::string, int>{}, ai_type, ai_config);
    } else {
        manager = std::make_unique<GameManager>(ai_type, ai_config, "human", std::map<std::string, int>{});
    }

    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "DÉBUT DE LA PARTIE - Seuls les coups seront affichés" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

    // Pas d'affichage du plateau initial
    // printGameBoard(manager->state);

    manager->playGame(true);

    // Affichage du résultat
    GameStats stats = manager->getGameStats();
    int winner = manager->state.getWinner();

    std::cout << "\n" << std::string(80, '=') << std::endl;
    if (winner == 0) {
        std::cout << "RÉSULTAT: ÉGALITÉ (" << stats.player1_captured << " - " << stats.player2_captured << ")" << std::endl;
    } else if (winner == human_player) {
        std::cout << "RÉSULTAT: VOUS AVEZ GAGNÉ! (" << stats.player1_captured << " - " << stats.player2_captured << ")" << std::endl;
    } else {
        std::cout << "RÉSULTAT: L'IA A GAGNÉ! (" << stats.player1_captured << " - " << stats.player2_captured << ")" << std::endl;
    }
    std::cout << std::string(80, '=') << std::endl;
}

void playAIVsAI() {
    /**Lance une partie IA vs IA*/
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "IA VS IA" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

    std::cout << "\nSélectionnez l'IA pour le Joueur 1 (trous impairs):" << std::endl;
    auto [ai1_type, ai1_config] = getAIChoice();

    std::cout << "\nSélectionnez l'IA pour le Joueur 2 (trous pairs):" << std::endl;
    auto [ai2_type, ai2_config] = getAIChoice();

    GameManager manager(ai1_type, ai1_config, ai2_type, ai2_config);

    // Convert to uppercase for display
    std::string ai1_upper = ai1_type;
    std::string ai2_upper = ai2_type;
    std::transform(ai1_upper.begin(), ai1_upper.end(), ai1_upper.begin(), ::toupper);
    std::transform(ai2_upper.begin(), ai2_upper.end(), ai2_upper.begin(), ::toupper);

    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "DÉBUT: " << ai1_upper << " (J1) vs " << ai2_upper << " (J2)" << std::endl;
    std::cout << "Seuls les coups seront affichés" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

    // Pas d'affichage du plateau initial
    // printGameBoard(manager.state);

    manager.playGame(true);

    // Affichage du résultat
    GameStats stats = manager.getGameStats();
    int winner = manager.state.getWinner();

    std::cout << "\n" << std::string(80, '=') << std::endl;
    if (winner == 0) {
        std::cout << "RÉSULTAT: ÉGALITÉ (" << stats.player1_captured << " - " << stats.player2_captured << ")" << std::endl;
    } else if (winner == 1) {
        std::cout << "RÉSULTAT: " << ai1_upper << " (J1) A GAGNÉ! (" << stats.player1_captured << " - " << stats.player2_captured << ")" << std::endl;
    } else {
        std::cout << "RÉSULTAT: " << ai2_upper << " (J2) A GAGNÉ! (" << stats.player1_captured << " - " << stats.player2_captured << ")" << std::endl;
    }
    std::cout << std::string(80, '=') << std::endl;
}

void playTournament() {
    /**Lance un tournoi entre les 5 IA*/
    // std::cout << "\n" << std::string(80, '=') << std::endl;
    // std::cout << "TOURNOI - TOUS LES IA SE BATTENT" << std::endl;
    // std::cout << std::string(80, '=') << std::endl;

    Tournament tournament;

    std::vector<std::pair<std::string, std::map<std::string, int>>> ais = {
        {"bfs", {{"depth", 2}}},
        {"dfs", {{"depth", 3}}},
        {"minimax", {{"depth", 4}}},
        {"alphabeta", {{"depth", 5}}},
        {"iddfs", {{"depth", 6}}}
    };

    std::vector<std::string> ai_names = {"BFS", "DFS", "Min-Max", "Alpha-Beta", "ID-DFS"};

    // std::cout << "\nOuverture du tournoi..." << std::endl;

    int match_count = 0;
    for (size_t i = 0; i < ais.size(); i++) {
        for (size_t j = 0; j < ais.size(); j++) {
            if (i < j) {  // Évite les doublons
                match_count++;
                // std::cout << "\nMatch " << match_count << ": " << ai_names[i] << " vs " << ai_names[j] << std::endl;

                GameManager manager(ais[i].first, ais[i].second, ais[j].first, ais[j].second);
                manager.playGame(false);

                GameStats stats = manager.getGameStats();
                int winner = manager.state.getWinner();

                if (winner == 0) {
                    std::cout << "  Résultat: ÉGALITÉ (" << stats.player1_captured << " - " << stats.player2_captured << ")" << std::endl;
                } else {
                    std::string winner_name = (winner == 1) ? ai_names[i] : ai_names[j];
                    std::cout << "  Gagnant: " << winner_name << " (" << stats.player1_captured << " - " << stats.player2_captured << ")" << std::endl;
                }
            }
        }
    }

    tournament.printResults();
}

int main() {
    /**Boucle principale*/
    // Force unbuffered output
    std::cout.sync_with_stdio(false);
    std::cout << std::unitbuf;
    
    while (true) {
        printMenu();

        try {
            std::string choice;
            std::getline(std::cin, choice);
            // Trim whitespace
            choice.erase(0, choice.find_first_not_of(" \t\n\r"));
            choice.erase(choice.find_last_not_of(" \t\n\r") + 1);

            if (choice == "1") {
                playHumanVsAI();
            } else if (choice == "2") {
                playAIVsAI();
            } else if (choice == "3") {
                playTournament();
            } else if (choice == "4") {
                std::cout << "\nAu revoir!" << std::endl;
                break;
            } else {
                std::cout << "Choix invalide! Entrez 1, 2, 3 ou 4." << std::endl;
            }

        } catch (...) {
            std::cout << "\n\nPartie annulée par l'utilisateur." << std::endl;
            break;
        }
    }

    return 0;
}
