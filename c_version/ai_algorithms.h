/**
 * Algorithmes d'IA pour le jeu Mancala
 * - BFS (Breadth-First Search)
 * - DFS (Depth-First Search)
 * - Min-Max avec Alpha-Beta Pruning
 * - Iterative Deepening DFS
 */

#ifndef AI_ALGORITHMS_H
#define AI_ALGORITHMS_H

#include "game_rules.h"
#include "game_engine.h"
#include <vector>
#include <tuple>
#include <optional>
#include <queue>
#include <limits>
#include <cmath>

class Evaluator {
public:
    /**Évalue la qualité d'une position*/

    static double evaluate(const GameState& state, int player) {
        /**
        Évalue une position pour un joueur
        Score positif = avantage pour le joueur
        Score négatif = avantage pour l'adversaire
        */
        int opponent = 3 - player;

        // Différence de graines capturées
        double score = (state.captured_seeds.at(player) - state.captured_seeds.at(opponent)) * 10.0;

        // Bonus pour les graines sur le plateau
        int player_seeds = 0;
        int opponent_seeds = 0;

        for (int hole : state.getPlayerHoles(player)) {
            player_seeds += state.getTotalSeeds(hole);
        }

        for (int hole : state.getPlayerHoles(opponent)) {
            opponent_seeds += state.getTotalSeeds(hole);
        }

        score += (player_seeds - opponent_seeds) * 2.0;

        return score;
    }

    static bool isTerminal(const GameState& state) {
        /**Vérifie si c'est un état terminal*/
        return state.isGameOver();
    }

    static double getTerminalScore(const GameState& state, int player) {
        /**Retourne le score d'un état terminal*/
        if (state.captured_seeds.at(player) > state.captured_seeds.at(3 - player)) {
            return std::numeric_limits<double>::infinity();  // Victoire
        } else if (state.captured_seeds.at(player) < state.captured_seeds.at(3 - player)) {
            return -std::numeric_limits<double>::infinity();  // Défaite
        } else {
            return 0.0;  // Égalité
        }
    }
};


// Structure pour représenter un coup
struct Move {
    int hole;
    Color color;
    Color transparent_as;
    bool use_transparent;
    bool valid;

    Move() : hole(-1), color(Color::RED), transparent_as(Color::RED), use_transparent(false), valid(false) {}
    Move(int h, Color c, Color t, bool ut) : hole(h), color(c), transparent_as(t), use_transparent(ut), valid(true) {}
};


class BFSBot {
public:
    /**Algorithme BFS pour explorer les états à profondeur égale*/
    int depth;
    Evaluator evaluator;

    BFSBot(int d = 2) : depth(d) {}

    Move search(const GameState& state, int player) {
        /**
        Effectue une recherche BFS jusqu'à une profondeur donnée
        Retourne le meilleur coup
        */
        if (evaluator.isTerminal(state)) {
            return Move();
        }

        // Queue: (state, depth, first_move)
        struct QueueItem {
            GameState state;
            int depth;
            Move first_move;
        };
        std::queue<QueueItem> queue;
        queue.push({state, 0, Move()});
        
        Move best_move;
        double best_score = -std::numeric_limits<double>::infinity();

        while (!queue.empty()) {
            QueueItem item = queue.front();
            queue.pop();

            // Si on a atteint la profondeur désirée
            if (item.depth == depth) {
                double score = evaluator.evaluate(item.state, player);
                if (score > best_score && item.first_move.valid) {
                    best_score = score;
                    best_move = item.first_move;
                }
                continue;
            }

            // Explorer les enfants
            if (item.state.current_player == player) {
                auto next_moves = MoveGenerator::getAllMoves(item.state, player);

                for (const auto& move : next_moves) {
                    int hole = std::get<0>(move);
                    Color color = std::get<1>(move);
                    Color trans_as = std::get<2>(move);
                    bool use_trans = std::get<3>(move);

                    GameState new_state = MoveGenerator::applyMove(item.state, hole, color, trans_as, use_trans);
                    Move first = item.first_move.valid ? item.first_move : Move(hole, color, trans_as, use_trans);
                    queue.push({new_state, item.depth + 1, first});
                }
            }
        }

        return best_move;
    }

    Move getMove(const GameState& state, int player) {
        /**Interface publique pour obtenir un coup*/
        return search(state, player);
    }
};


class DFSBot {
public:
    /**Algorithme DFS pour explorer les états en profondeur*/
    int depth;
    Evaluator evaluator;

    DFSBot(int d = 3) : depth(d) {}

    std::pair<double, Move> search(const GameState& state, int player, int current_depth = 0) {
        /**
        Effectue une recherche DFS
        Retourne (meilleur_score, meilleur_coup)
        */
        // État terminal
        if (evaluator.isTerminal(state)) {
            int opponent = 3 - player;
            if (state.captured_seeds.at(player) > state.captured_seeds.at(opponent)) {
                return {std::numeric_limits<double>::infinity(), Move()};
            } else if (state.captured_seeds.at(player) < state.captured_seeds.at(opponent)) {
                return {-std::numeric_limits<double>::infinity(), Move()};
            } else {
                return {0.0, Move()};
            }
        }

        // Profondeur atteinte
        if (current_depth >= depth) {
            return {evaluator.evaluate(state, player), Move()};
        }

        double best_score;
        Move best_move;

        if (state.current_player == player) {
            // Joueur maximisant
            best_score = -std::numeric_limits<double>::infinity();
            auto moves = MoveGenerator::getAllMoves(state, player);

            for (const auto& move : moves) {
                int hole = std::get<0>(move);
                Color color = std::get<1>(move);
                Color trans_as = std::get<2>(move);
                bool use_trans = std::get<3>(move);

                GameState new_state = MoveGenerator::applyMove(state, hole, color, trans_as, use_trans);
                auto [score, _] = search(new_state, player, current_depth + 1);

                if (score > best_score) {
                    best_score = score;
                    best_move = Move(hole, color, trans_as, use_trans);
                }
            }
        } else {
            // Joueur minimisant
            int opponent = 3 - player;
            best_score = std::numeric_limits<double>::infinity();
            auto moves = MoveGenerator::getAllMoves(state, opponent);

            for (const auto& move : moves) {
                int hole = std::get<0>(move);
                Color color = std::get<1>(move);
                Color trans_as = std::get<2>(move);
                bool use_trans = std::get<3>(move);

                GameState new_state = MoveGenerator::applyMove(state, hole, color, trans_as, use_trans);
                auto [score, _] = search(new_state, player, current_depth + 1);

                if (score < best_score) {
                    best_score = score;
                    best_move = Move(hole, color, trans_as, use_trans);
                }
            }
        }

        return {best_score, best_move};
    }

    Move getMove(const GameState& state, int player) {
        /**Interface publique pour obtenir un coup*/
        auto [_, move] = search(state, player, 0);
        return move;
    }
};


class MinMaxBot {
public:
    /**Algorithme Min-Max avec Alpha-Beta Pruning*/
    int depth;
    Evaluator evaluator;
    int nodes_explored;

    MinMaxBot(int d = 4) : depth(d), nodes_explored(0) {}

    std::pair<double, Move> search(const GameState& state, int player, int current_depth = 0,
                                    double alpha = -std::numeric_limits<double>::infinity(),
                                    double beta = std::numeric_limits<double>::infinity()) {
        /**
        Min-Max avec Alpha-Beta Pruning
        */
        nodes_explored++;

        // État terminal
        if (evaluator.isTerminal(state)) {
            return {evaluator.getTerminalScore(state, player), Move()};
        }

        // Profondeur atteinte
        if (current_depth >= depth) {
            return {evaluator.evaluate(state, player), Move()};
        }

        if (state.current_player == player) {
            // Nœud maximisant
            double max_eval = -std::numeric_limits<double>::infinity();
            Move best_move;

            auto moves = MoveGenerator::getAllMoves(state, player);

            for (const auto& move : moves) {
                int hole = std::get<0>(move);
                Color color = std::get<1>(move);
                Color trans_as = std::get<2>(move);
                bool use_trans = std::get<3>(move);

                GameState new_state = MoveGenerator::applyMove(state, hole, color, trans_as, use_trans);
                auto [eval_score, _] = search(new_state, player, current_depth + 1, alpha, beta);

                if (eval_score > max_eval) {
                    max_eval = eval_score;
                    best_move = Move(hole, color, trans_as, use_trans);
                }

                alpha = std::max(alpha, eval_score);
                if (beta <= alpha) {
                    break;  // Beta cutoff
                }
            }

            return {max_eval, best_move};
        } else {
            // Nœud minimisant
            int opponent = 3 - player;
            double min_eval = std::numeric_limits<double>::infinity();
            Move best_move;

            auto moves = MoveGenerator::getAllMoves(state, opponent);

            for (const auto& move : moves) {
                int hole = std::get<0>(move);
                Color color = std::get<1>(move);
                Color trans_as = std::get<2>(move);
                bool use_trans = std::get<3>(move);

                GameState new_state = MoveGenerator::applyMove(state, hole, color, trans_as, use_trans);
                auto [eval_score, _] = search(new_state, player, current_depth + 1, alpha, beta);

                if (eval_score < min_eval) {
                    min_eval = eval_score;
                    best_move = Move(hole, color, trans_as, use_trans);
                }

                beta = std::min(beta, eval_score);
                if (beta <= alpha) {
                    break;  // Alpha cutoff
                }
            }

            return {min_eval, best_move};
        }
    }

    Move getMove(const GameState& state, int player) {
        /**Interface publique pour obtenir un coup*/
        nodes_explored = 0;
        auto [_, move] = search(state, player, 0);
        return move;
    }
};


class AlphaBetaBot {
public:
    /**Algorithme Alpha-Beta Pruning - Version optimisée de MinMax*/
    int depth;
    Evaluator evaluator;
    int nodes_explored;
    int pruned_branches;

    AlphaBetaBot(int d = 5) : depth(d), nodes_explored(0), pruned_branches(0) {}

    std::pair<double, Move> search(const GameState& state, int player, int current_depth = 0,
                                    double alpha = -std::numeric_limits<double>::infinity(),
                                    double beta = std::numeric_limits<double>::infinity(),
                                    bool is_maximizing = true) {
        /**
        Alpha-Beta Pruning - Élagage des branches non prometteuses

        Alpha: Meilleure valeur pour le joueur maximisant
        Beta: Meilleure valeur pour le joueur minimisant
        */
        nodes_explored++;

        // État terminal
        if (evaluator.isTerminal(state)) {
            double terminal_score = evaluator.getTerminalScore(state, player);
            return {terminal_score, Move()};
        }

        // Profondeur atteinte
        if (current_depth >= depth) {
            return {evaluator.evaluate(state, player), Move()};
        }

        int current_player = state.current_player;

        if (is_maximizing) {
            // Nœud maximisant (notre joueur)
            double max_eval = -std::numeric_limits<double>::infinity();
            Move best_move;

            auto moves = MoveGenerator::getAllMoves(state, current_player);

            for (const auto& move : moves) {
                int hole = std::get<0>(move);
                Color color = std::get<1>(move);
                Color trans_as = std::get<2>(move);
                bool use_trans = std::get<3>(move);

                GameState new_state = MoveGenerator::applyMove(state, hole, color, trans_as, use_trans);

                // Déterminer si le prochain niveau est maximisant ou minimisant
                bool next_is_maximizing = (new_state.current_player == player);

                auto [eval_score, _] = search(new_state, player, current_depth + 1, alpha, beta, next_is_maximizing);

                if (eval_score > max_eval) {
                    max_eval = eval_score;
                    best_move = Move(hole, color, trans_as, use_trans);
                }

                // Mise à jour d'alpha
                alpha = std::max(alpha, eval_score);

                // Élagage Beta
                if (beta <= alpha) {
                    pruned_branches++;
                    break;  // Beta cutoff - on peut arrêter d'explorer cette branche
                }
            }

            return {max_eval, best_move};
        } else {
            // Nœud minimisant (adversaire)
            double min_eval = std::numeric_limits<double>::infinity();
            Move best_move;

            auto moves = MoveGenerator::getAllMoves(state, current_player);

            for (const auto& move : moves) {
                int hole = std::get<0>(move);
                Color color = std::get<1>(move);
                Color trans_as = std::get<2>(move);
                bool use_trans = std::get<3>(move);

                GameState new_state = MoveGenerator::applyMove(state, hole, color, trans_as, use_trans);

                // Déterminer si le prochain niveau est maximisant ou minimisant
                bool next_is_maximizing = (new_state.current_player == player);

                auto [eval_score, _] = search(new_state, player, current_depth + 1, alpha, beta, next_is_maximizing);

                if (eval_score < min_eval) {
                    min_eval = eval_score;
                    best_move = Move(hole, color, trans_as, use_trans);
                }

                // Mise à jour de beta
                beta = std::min(beta, eval_score);

                // Élagage Alpha
                if (beta <= alpha) {
                    pruned_branches++;
                    break;  // Alpha cutoff - on peut arrêter d'explorer cette branche
                }
            }

            return {min_eval, best_move};
        }
    }

    Move getMove(const GameState& state, int player) {
        /**Interface publique pour obtenir un coup*/
        nodes_explored = 0;
        pruned_branches = 0;

        auto [_, move] = search(state, player, 0, -std::numeric_limits<double>::infinity(), 
                                 std::numeric_limits<double>::infinity(), true);

        // std::cout << "[Alpha-Beta] Nœuds explorés: " << nodes_explored 
        //           << ", Branches élaguées: " << pruned_branches << std::endl;

        return move;
    }
};


class IterativeDeepeningDFSBot {
public:
    /**Algorithme Iterative Deepening DFS*/
    int max_depth;
    Evaluator evaluator;
    int nodes_explored;

    IterativeDeepeningDFSBot(int d = 6) : max_depth(d), nodes_explored(0) {}

    std::pair<double, Move> dfsLimited(const GameState& state, int player, int depth_limit, int current_depth = 0) {
        /**
        DFS avec limite de profondeur
        */
        nodes_explored++;

        // État terminal
        if (evaluator.isTerminal(state)) {
            return {evaluator.getTerminalScore(state, player), Move()};
        }

        // Limite de profondeur atteinte
        if (current_depth >= depth_limit) {
            return {evaluator.evaluate(state, player), Move()};
        }

        double best_score;
        Move best_move;

        if (state.current_player == player) {
            // Maximisant
            best_score = -std::numeric_limits<double>::infinity();
            auto moves = MoveGenerator::getAllMoves(state, player);

            for (const auto& move : moves) {
                int hole = std::get<0>(move);
                Color color = std::get<1>(move);
                Color trans_as = std::get<2>(move);
                bool use_trans = std::get<3>(move);

                GameState new_state = MoveGenerator::applyMove(state, hole, color, trans_as, use_trans);
                auto [score, _] = dfsLimited(new_state, player, depth_limit, current_depth + 1);

                if (score > best_score) {
                    best_score = score;
                    best_move = Move(hole, color, trans_as, use_trans);
                }
            }
        } else {
            // Minimisant
            int opponent = 3 - player;
            best_score = std::numeric_limits<double>::infinity();
            auto moves = MoveGenerator::getAllMoves(state, opponent);

            for (const auto& move : moves) {
                int hole = std::get<0>(move);
                Color color = std::get<1>(move);
                Color trans_as = std::get<2>(move);
                bool use_trans = std::get<3>(move);

                GameState new_state = MoveGenerator::applyMove(state, hole, color, trans_as, use_trans);
                auto [score, _] = dfsLimited(new_state, player, depth_limit, current_depth + 1);

                if (score < best_score) {
                    best_score = score;
                    best_move = Move(hole, color, trans_as, use_trans);
                }
            }
        }

        return {best_score, best_move};
    }

    Move search(const GameState& state, int player) {
        /**
        Iterative Deepening: augmente progressivement la profondeur
        */
        Move best_move;

        for (int d = 1; d <= max_depth; d++) {
            nodes_explored = 0;
            auto [_, move] = dfsLimited(state, player, d);

            if (move.valid) {
                best_move = move;
                // std::cout << "[ID-DFS] Profondeur " << d << ": " << nodes_explored << " nœuds explorés" << std::endl;
            }
        }

        return best_move;
    }

    Move getMove(const GameState& state, int player) {
        /**Interface publique pour obtenir un coup*/
        return search(state, player);
    }
};

#endif // AI_ALGORITHMS_H
