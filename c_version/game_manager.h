/**
 * Gestionnaire de jeu - Gère les parties et l'interaction entre joueurs et IA
 */

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "game_rules.h"
#include "game_engine.h"
#include "ai_algorithms.h"
#include "config.h"
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <memory>

// Forward declarations
class AIPlayer;
class HumanPlayer;

// Base class for players
class Player {
public:
    int player_id;
    virtual ~Player() = default;
    virtual Move getMove(GameState& state) = 0;
    virtual bool isHuman() const = 0;
};


class AIPlayer : public Player {
public:
    /**Représente un joueur IA*/
    std::string algorithm_name;
    int depth;
    
    // AI instances
    std::unique_ptr<BFSBot> bfs_bot;
    std::unique_ptr<DFSBot> dfs_bot;
    std::unique_ptr<MinMaxBot> minmax_bot;
    std::unique_ptr<AlphaBetaBot> alphabeta_bot;
    std::unique_ptr<IterativeDeepeningDFSBot> iddfs_bot;

    AIPlayer(int id, const std::string& algo_name, int d = 3) 
        : algorithm_name(algo_name), depth(d) {
        player_id = id;
        createAI(algo_name, d);
    }

    bool isHuman() const override { return false; }

    void createAI(const std::string& algo_name, int d) {
        /**Crée l'instance d'IA appropriée*/
        std::string lower_name = algo_name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);

        if (lower_name == "bfs") {
            bfs_bot = std::make_unique<BFSBot>(d);
        } else if (lower_name == "dfs") {
            dfs_bot = std::make_unique<DFSBot>(d);
        } else if (lower_name == "minimax") {
            minmax_bot = std::make_unique<MinMaxBot>(d);
        } else if (lower_name == "alphabeta") {
            alphabeta_bot = std::make_unique<AlphaBetaBot>(d);
        } else if (lower_name == "iddfs") {
            iddfs_bot = std::make_unique<IterativeDeepeningDFSBot>(d);
        }
    }

    Move getMove(GameState& state) override {
        /**Retourne le meilleur coup selon l'algorithme*/
        std::string lower_name = algorithm_name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);

        if (lower_name == "bfs" && bfs_bot) {
            return bfs_bot->getMove(state, player_id);
        } else if (lower_name == "dfs" && dfs_bot) {
            return dfs_bot->getMove(state, player_id);
        } else if (lower_name == "minimax" && minmax_bot) {
            return minmax_bot->getMove(state, player_id);
        } else if (lower_name == "alphabeta" && alphabeta_bot) {
            return alphabeta_bot->getMove(state, player_id);
        } else if (lower_name == "iddfs" && iddfs_bot) {
            return iddfs_bot->getMove(state, player_id);
        }
        return Move();
    }
};


class HumanPlayer : public Player {
public:
    /**Représente un joueur humain*/

    HumanPlayer(int id) {
        player_id = id;
    }

    bool isHuman() const override { return true; }

    std::optional<std::tuple<int, Color, Color, bool>> parseMoveInput(const std::string& move_str, const GameState& state) {
        /**
        Parse l'entrée utilisateur au format simple
        Formats acceptés:
        - "14B" : Trou 14, couleur BLEU
        - "3R" : Trou 3, couleur ROUGE
        - "5TR" : Trou 5, graines TRANSPARENTES comme ROUGE
        - "8TB" : Trou 8, graines TRANSPARENTES comme BLEU
        */
        std::string upper_str = move_str;
        // Trim whitespace
        upper_str.erase(0, upper_str.find_first_not_of(" \t\n\r"));
        upper_str.erase(upper_str.find_last_not_of(" \t\n\r") + 1);
        // Convert to uppercase
        std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);

        // Parser le format: numero + couleur(s)
        if (upper_str.length() >= 3 && upper_str.substr(upper_str.length() - 2) == "TR") {
            // Transparent comme Rouge
            try {
                int hole = std::stoi(upper_str.substr(0, upper_str.length() - 2));
                return std::make_tuple(hole, Color::TRANSPARENT, Color::RED, true);
            } catch (...) {
                return std::nullopt;
            }
        } else if (upper_str.length() >= 3 && upper_str.substr(upper_str.length() - 2) == "TB") {
            // Transparent comme Bleu
            try {
                int hole = std::stoi(upper_str.substr(0, upper_str.length() - 2));
                return std::make_tuple(hole, Color::TRANSPARENT, Color::BLUE, true);
            } catch (...) {
                return std::nullopt;
            }
        } else if (upper_str.length() >= 2 && upper_str.back() == 'R') {
            // Rouge
            try {
                int hole = std::stoi(upper_str.substr(0, upper_str.length() - 1));
                return std::make_tuple(hole, Color::RED, Color::RED, false);
            } catch (...) {
                return std::nullopt;
            }
        } else if (upper_str.length() >= 2 && upper_str.back() == 'B') {
            // Bleu
            try {
                int hole = std::stoi(upper_str.substr(0, upper_str.length() - 1));
                return std::make_tuple(hole, Color::BLUE, Color::BLUE, false);
            } catch (...) {
                return std::nullopt;
            }
        }

        return std::nullopt;
    }

    bool isMoveValid(int hole, Color color, const GameState& state) {
        /**Vérifie si un coup est valide*/
        // Vérifier que le trou est dans les trous du joueur
        std::vector<int> player_holes = state.getPlayerHoles(player_id);
        if (std::find(player_holes.begin(), player_holes.end(), hole) == player_holes.end()) {
            return false;
        }

        // Vérifier que le trou a des graines de cette couleur
        if (state.holes.at(hole).at(color) == 0) {
            return false;
        }

        return true;
    }

    Move getMove(GameState& state) override {
        /**Demande un coup au joueur avec le nouveau format*/
        // std::cout << "\n" << std::string(80, '=') << std::endl;
        // std::cout << "Au tour du Joueur " << player_id << std::endl;
        // std::cout << "\nVos trous: ";
        // for (int h : state.getPlayerHoles(player_id)) std::cout << h << " ";
        // std::cout << std::endl;
        // std::cout << "\nCoups valides (exemples):" << std::endl;
        // std::cout << "  14B  -> Trou 14, couleur BLEU" << std::endl;
        // std::cout << "  3R   -> Trou 3, couleur ROUGE" << std::endl;
        // std::cout << "  5TR  -> Trou 5, graines TRANSPARENTES comme ROUGE" << std::endl;
        // std::cout << "  8TB  -> Trou 8, graines TRANSPARENTES comme BLEU" << std::endl;

        // Afficher les graines disponibles EN PARALLÈLE (horizontalement)
        // std::cout << "\n" << std::string(150, '=') << std::endl;
        // std::cout << "GRAINES DISPONIBLES - VUE PARALLÈLE:" << std::endl;
        // std::cout << std::string(150, '=') << std::endl;

        int opponent = 3 - player_id;
        std::vector<int> player_holes = state.getPlayerHoles(player_id);
        std::vector<int> opponent_holes = state.getPlayerHoles(opponent);

        // En-têtes
        // std::cout << "\nVOS TROUS (Joueur " << player_id << "):";
        // ... (display code commented out as in Python)

        while (true) {
            try {
                std::string move_input;
                std::cout << "\nEntrez votre coup (ex: 14B, 3R, 5TR, 8TB): ";
                std::getline(std::cin, move_input);

                // Parser l'entrée
                auto parsed = parseMoveInput(move_input, state);

                if (!parsed.has_value()) {
                    // std::cout << "Format invalide! Utilisez le format: [TROU][COULEUR]" << std::endl;
                    // std::cout << "Exemples: 14B, 3R, 5TR, 8TB" << std::endl;
                    continue;
                }

                auto [hole, color, trans_as, use_trans] = parsed.value();

                // Vérifier la validité du coup
                if (!isMoveValid(hole, color, state)) {
                    // std::cout << "Coup invalide! Le trou " << hole << " n'a pas de graines " 
                    //           << colorToString(color) << std::endl;
                    continue;
                }

                return Move(hole, color, trans_as, use_trans);

            } catch (...) {
                // std::cout << "Erreur! Veuillez réessayer!" << std::endl;
                continue;
            }
        }
    }
};


struct MoveRecord {
    int player;
    int hole;
    std::string color;
    std::string transparent_as;
    double time;
    bool timeout;
};


struct GameStats {
    int total_moves;
    int player1_captured;
    int player2_captured;
    int seeds_on_board;
    std::vector<MoveRecord> move_history;
};


class GameManager {
public:
    /**Gère une partie complète*/
    GameState state;
    GameEngine engine;
    std::map<int, std::unique_ptr<Player>> players;
    std::vector<MoveRecord> move_history;
    int total_moves;

    GameManager(const std::string& player1_type, const std::map<std::string, int>& player1_config,
                const std::string& player2_type, const std::map<std::string, int>& player2_config)
        : engine(&state), total_moves(0) {
        /**
        player_type: "human" ou nom de l'algorithme ("bfs", "dfs", "minimax", "iddfs")
        player_config: map avec "depth" pour les IA
        */
        createPlayer(1, player1_type, player1_config);
        createPlayer(2, player2_type, player2_config);
    }

    void createPlayer(int player_id, const std::string& player_type, const std::map<std::string, int>& config) {
        /**Crée un joueur selon le type*/
        std::string lower_type = player_type;
        std::transform(lower_type.begin(), lower_type.end(), lower_type.begin(), ::tolower);

        if (lower_type == "human") {
            players[player_id] = std::make_unique<HumanPlayer>(player_id);
        } else {
            int depth = 3;
            auto it = config.find("depth");
            if (it != config.end()) {
                depth = it->second;
            }
            players[player_id] = std::make_unique<AIPlayer>(player_id, player_type, depth);
        }
    }

    bool playTurn(bool verbose = true) {
        /**
        Exécute un tour pour le joueur courant
        Retourne True si le coup a été joué, False si le jeu est terminé
        */
        if (state.isGameOver()) {
            return false;
        }

        int player_id = state.current_player;
        Player* player = players[player_id].get();

        // Affiche le plateau de manière claire
        if (verbose) {
            printBoard();
        }

        // Obtient le coup
        auto start_time = std::chrono::high_resolution_clock::now();
        Move move = player->getMove(state);
        auto end_time = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(end_time - start_time).count();

        // Vérifier si le joueur a dépassé le temps limite
        if (elapsed > MAX_TIME_PER_MOVE) {
            if (verbose) {
                // std::cout << "\n❌ TIMEOUT! Joueur " << player_id << " a dépassé le temps limite!" << std::endl;
                // std::cout << "   Temps utilisé: " << elapsed << "s (Limite: " << MAX_TIME_PER_MOVE << "s)" << std::endl;
                // std::cout << "   Joueur " << player_id << " PERD par timeout!" << std::endl;
            }

            // Forcer la fin du jeu avec une défaite pour le joueur qui a timeout
            int opponent = 3 - player_id;
            state.captured_seeds[opponent] = 49;  // L'adversaire gagne automatiquement

            move_history.push_back({player_id, -1, "", "", elapsed, true});

            return false;
        }

        if (!move.valid) {
            // std::cout << "Aucun coup valide pour le joueur " << player_id << std::endl;
            return false;
        }

        if (!engine.playMove(move.hole, move.color, move.transparent_as, move.use_transparent)) {
            // std::cout << "Coup invalide: " << move.hole << colorToString(move.color) << std::endl;
            return false;
        }

        move_history.push_back({
            player_id,
            move.hole,
            colorToString(move.color),
            move.use_transparent ? colorToString(move.transparent_as) : "",
            elapsed,
            false
        });
        total_moves++;

        if (verbose) {
            if (move.use_transparent) {
                std::cout << move.hole << "T" << colorToString(move.transparent_as) << std::endl;
            } else {
                std::cout << move.hole << colorToString(move.color) << std::endl;
            }
            // std::cout << "   Temps de calcul: " << elapsed << "s" << std::endl;
        }

        return true;
    }

    void printBoard() {
        /**Affiche le plateau de manière claire et formatée*/
        // std::cout << "\n" << std::string(100, '=') << std::endl;
        // std::cout << "Score P1: " << state.captured_seeds[1] << " | Score P2: " << state.captured_seeds[2] << std::endl;

        // // Ligne 1 : Trous 1-8
        // std::cout << "\n| ";
        // for (int hole = 1; hole <= 8; hole++) {
        //     std::cout << "Field:" << hole << "| ";
        // }
        // std::cout << std::endl;

        // // Ligne 2 : Graines ROUGES (trous 1-8)
        // std::cout << "| ";
        // for (int hole = 1; hole <= 8; hole++) {
        //     int red_count = state.holes[hole][Color::RED];
        //     std::cout << "RED:" << red_count << "  | ";
        // }
        // std::cout << std::endl;

        // // Ligne 3 : Graines BLEUES (trous 1-8)
        // std::cout << "| ";
        // for (int hole = 1; hole <= 8; hole++) {
        //     int blue_count = state.holes[hole][Color::BLUE];
        //     std::cout << "BLUE:" << blue_count << " | ";
        // }
        // std::cout << std::endl;

        // // Ligne 4 : Graines TRANSPARENTES (trous 1-8)
        // std::cout << "| ";
        // for (int hole = 1; hole <= 8; hole++) {
        //     int trans_count = state.holes[hole][Color::TRANSPARENT];
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
        //     int red_count = state.holes[hole][Color::RED];
        //     std::cout << "RED:" << red_count << "  | ";
        // }
        // std::cout << std::endl;

        // // Ligne 7 : Graines BLEUES (trous 9-16)
        // std::cout << "| ";
        // for (int hole = 9; hole <= 16; hole++) {
        //     int blue_count = state.holes[hole][Color::BLUE];
        //     std::cout << "BLUE:" << blue_count << " | ";
        // }
        // std::cout << std::endl;

        // // Ligne 8 : Graines TRANSPARENTES (trous 9-16)
        // std::cout << "| ";
        // for (int hole = 9; hole <= 16; hole++) {
        //     int trans_count = state.holes[hole][Color::TRANSPARENT];
        //     std::cout << "TRSP:" << trans_count << " | ";
        // }
        // std::cout << std::endl;

        // std::cout << "\n" << std::string(100, '=') << std::endl;
    }

    int playGame(bool verbose = true, int max_turns = 400) {
        /**
        Joue une partie complète
        Retourne l'ID du gagnant (0 pour égalité)
        */
        int turn_count = 0;

        while (!state.isGameOver() && turn_count < max_turns) {
            if (!playTurn(verbose)) {
                break;
            }
            turn_count++;
        }

        // if (verbose) {
        //     std::cout << "\n" << std::string(80, '=') << std::endl;
        //     std::cout << "FIN DE LA PARTIE" << std::endl;
        //     std::cout << std::string(80, '=') << std::endl;
        //     std::cout << "Nombre de coups joués: " << state.move_count << std::endl;

        //     // Vérifier si la partie s'est terminée par timeout
        //     if (!move_history.empty() && move_history.back().timeout) {
        //         int timeout_player = move_history.back().player;
        //         std::cout << "Raison: Joueur " << timeout_player << " a dépassé le temps limite (" 
        //                   << MAX_TIME_PER_MOVE << "s)!" << std::endl;
        //     }
        //     // Afficher la raison de fin de partie
        //     else if (state.move_count >= state.MAX_MOVES) {
        //         std::cout << "Raison: Limite de " << state.MAX_MOVES << " coups atteinte!" << std::endl;
        //     } else if (state.captured_seeds[1] >= 49) {
        //         std::cout << "Raison: Joueur 1 a capturé 49+ graines!" << std::endl;
        //     } else if (state.captured_seeds[2] >= 49) {
        //         std::cout << "Raison: Joueur 2 a capturé 49+ graines!" << std::endl;
        //     } else if (state.captured_seeds[1] >= 40 && state.captured_seeds[2] >= 40) {
        //         std::cout << "Raison: Les deux joueurs ont 40+ graines!" << std::endl;
        //     } else if (state.getSeedsOnBoard() < 10) {
        //         std::cout << "Raison: Moins de 10 graines sur le plateau!" << std::endl;
        //     }

        //     std::cout << state.toString() << std::endl;

        //     int winner = state.getWinner();
        //     if (winner == 0) {
        //         std::cout << "ÉGALITÉ!" << std::endl;
        //     } else {
        //         std::cout << "JOUEUR " << winner << " GAGNE!" << std::endl;
        //     }
        // }

        return state.getWinner();
    }

    GameStats getGameStats() {
        /**Retourne les statistiques de la partie*/
        return {
            total_moves,
            state.captured_seeds[1],
            state.captured_seeds[2],
            state.getSeedsOnBoard(),
            move_history
        };
    }
};


class Tournament {
public:
    /**Gère un tournoi entre plusieurs IA*/
    std::vector<std::map<std::string, std::string>> results;

    void playMatch(const std::string& player1_type, const std::map<std::string, int>& player1_config,
                   const std::string& player2_type, const std::map<std::string, int>& player2_config,
                   bool verbose = false) {
        /**Joue un match entre deux joueurs*/
        GameManager manager(player1_type, player1_config, player2_type, player2_config);
        int winner = manager.playGame(verbose);

        GameStats stats = manager.getGameStats();
        
        std::map<std::string, std::string> match_result;
        match_result["player1"] = player1_type;
        match_result["player2"] = player2_type;
        match_result["winner"] = std::to_string(winner);
        match_result["player1_captured"] = std::to_string(stats.player1_captured);
        match_result["player2_captured"] = std::to_string(stats.player2_captured);

        results.push_back(match_result);
    }

    void printResults() {
        /**Affiche les résultats du tournoi*/
        // std::cout << "\n" << std::string(80, '=') << std::endl;
        // std::cout << "RÉSULTATS DU TOURNOI" << std::endl;
        // std::cout << std::string(80, '=') << std::endl;

        // int i = 1;
        // for (const auto& result : results) {
        //     std::cout << "\nMatch " << i++ << ":" << std::endl;
        //     std::cout << "  " << result.at("player1") << " vs " << result.at("player2") << std::endl;

        //     if (result.at("winner") == "0") {
        //         std::cout << "  Résultat: ÉGALITÉ" << std::endl;
        //     } else {
        //         std::string winner_name = result.at("winner") == "1" ? result.at("player1") : result.at("player2");
        //         std::cout << "  Gagnant: Joueur " << result.at("winner") << " (" << winner_name << ")" << std::endl;
        //     }

        //     std::cout << "  Graines capturées - J1: " << result.at("player1_captured") 
        //               << ", J2: " << result.at("player2_captured") << std::endl;
        // }
    }
};

#endif // GAME_MANAGER_H
