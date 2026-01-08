/**
 * Moteur de jeu Mancala - Version CORRIGÉE avec règles complètes de capture
 * Les graines transparentes sont distribuées AVANT les autres graines de la couleur désignée
 */

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "game_rules.h"
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>

class GameEngine {
public:
    GameState* state;
    bool debug;  // Mettre à true pour voir le détail des captures

    GameEngine(GameState* game_state) : state(game_state), debug(false) {}

    int nextHole(int current_hole) const {
        /**Retourne le trou suivant en sens horaire*/
        if (current_hole == 16) {
            return 1;
        } else {
            return current_hole + 1;
        }
    }

    int prevHole(int current_hole) const {
        /**Retourne le trou précédent (sens anti-horaire)*/
        if (current_hole == 1) {
            return 16;
        } else {
            return current_hole - 1;
        }
    }

    bool isOpponentHole(int hole, int player) const {
        /**Vérifie si un trou appartient à l'adversaire*/
        std::vector<int> player_holes = state->getPlayerHoles(player);
        return std::find(player_holes.begin(), player_holes.end(), hole) == player_holes.end();
    }

    bool playMove(int hole, Color color, Color transparent_as = Color::RED, bool use_transparent = false) {
        /**
        Exécute un coup avec les règles complètes :
        - Si les graines sont rouges : distribuées dans tous les trous
        - Si les graines sont bleues : distribuées uniquement dans les trous adverses
        - Si transparentes : jouées COMME la couleur désignée (suivent ses règles de distribution)
          MAIS restent transparentes sur le plateau

        IMPORTANT: Les graines transparentes ne sont jouées QUE si color == Color::TRANSPARENT
        Quand on demande "1B", SEULES les graines BLEUES sont jouées, pas les transparentes.
        */
        int player = state->current_player;

        // Vérification de validité
        if (state->holes[hole][color] == 0) {
            return false;
        }

        std::vector<int> player_holes = state->getPlayerHoles(player);
        if (std::find(player_holes.begin(), player_holes.end(), hole) == player_holes.end()) {
            return false;
        }

        if (color == Color::TRANSPARENT && !use_transparent) {
            return false;
        }

        int last_hole_seeded = -1;
        Color distribution_rule;

        // Déterminer les graines à distribuer
        if (color == Color::TRANSPARENT) {
            // Les graines transparentes + les graines de la couleur désignée sont distribuées ensemble
            int trans_seeds = state->holes[hole][Color::TRANSPARENT];
            int color_seeds = state->holes[hole][transparent_as];

            distribution_rule = transparent_as;  // Règle de distribution (RED ou BLUE)

            int seeds_transparent = trans_seeds;
            int seeds_colored = color_seeds;

            state->holes[hole][Color::TRANSPARENT] = 0;
            state->holes[hole][transparent_as] = 0;

            if (debug) {
                std::cout << "\n[DEBUG] Joueur " << player << " joue trou " << hole 
                          << ", TRANSPARENT comme " << colorToString(transparent_as) << std::endl;
                std::cout << "[DEBUG] Graines transparentes: " << seeds_transparent 
                          << ", Graines " << colorToString(transparent_as) << ": " << seeds_colored << std::endl;
            }

            // Distribution spéciale: transparentes d'abord, puis colorées
            int trans_remaining = seeds_transparent;
            int colored_remaining = seeds_colored;
            int current_hole = hole;

            while (trans_remaining > 0 || colored_remaining > 0) {
                current_hole = nextHole(current_hole);

                // Déterminer quelle graine distribuer (transparente d'abord)
                Color current_seed_color;
                if (trans_remaining > 0) {
                    current_seed_color = Color::TRANSPARENT;
                    trans_remaining--;
                } else {
                    current_seed_color = transparent_as;
                    colored_remaining--;
                }

                // Distribution selon les règles
                if (distribution_rule == Color::RED) {
                    // Les rouges vont dans tous les trous
                    state->holes[current_hole][current_seed_color]++;
                    last_hole_seeded = current_hole;
                    if (debug) {
                        std::cout << "[DEBUG] Graine " << colorToString(current_seed_color) 
                                  << " -> Trou " << current_hole << " (règle RED)" << std::endl;
                    }
                } else if (distribution_rule == Color::BLUE) {
                    // Les bleus vont uniquement dans les trous adverses
                    if (isOpponentHole(current_hole, player)) {
                        state->holes[current_hole][current_seed_color]++;
                        last_hole_seeded = current_hole;
                        if (debug) {
                            std::cout << "[DEBUG] Graine " << colorToString(current_seed_color) 
                                      << " -> Trou adversaire " << current_hole << " (règle BLUE)" << std::endl;
                        }
                    } else {
                        // Si ce n'est pas un trou adverse, on remet la graine et on continue
                        if (current_seed_color == Color::TRANSPARENT) {
                            trans_remaining++;
                        } else {
                            colored_remaining++;
                        }
                    }
                }
            }
        } else {
            // Distribution normale pour RED ou BLUE seul
            int seeds_to_distribute = state->holes[hole][color];
            distribution_rule = color;
            Color seed_color = color;
            state->holes[hole][color] = 0;

            int current_hole = hole;
            int seeds_remaining = seeds_to_distribute;

            if (debug) {
                std::cout << "\n[DEBUG] Joueur " << player << " joue trou " << hole 
                          << ", couleur " << colorToString(seed_color) << std::endl;
                std::cout << "[DEBUG] Distribution selon les règles: " << colorToString(distribution_rule) << std::endl;
                std::cout << "[DEBUG] Graines à distribuer: " << seeds_to_distribute << std::endl;
            }

            while (seeds_remaining > 0) {
                current_hole = nextHole(current_hole);

                // Distribution selon les règles de distribution_rule
                if (distribution_rule == Color::RED) {
                    // Les rouges vont dans tous les trous
                    state->holes[current_hole][seed_color]++;
                    last_hole_seeded = current_hole;
                    if (debug) {
                        std::cout << "[DEBUG] Graine " << colorToString(seed_color) 
                                  << " -> Trou " << current_hole << " (règle RED)" << std::endl;
                    }
                    seeds_remaining--;
                } else if (distribution_rule == Color::BLUE) {
                    // Les bleus vont uniquement dans les trous adverses
                    if (isOpponentHole(current_hole, player)) {
                        state->holes[current_hole][seed_color]++;
                        last_hole_seeded = current_hole;
                        if (debug) {
                            std::cout << "[DEBUG] Graine " << colorToString(seed_color) 
                                      << " -> Trou adversaire " << current_hole << " (règle BLUE)" << std::endl;
                        }
                        seeds_remaining--;
                    }
                }
            }
        }

        // Capture à partir du dernier trou semé (si une graine a été semée)
        if (last_hole_seeded != -1) {
            captureSeeds(last_hole_seeded, player);
        }

        // Incrémenter le compteur de coups
        state->move_count++;

        // Changement de joueur
        state->current_player = 3 - player;  // Alterne entre 1 et 2

        return true;
    }

private:
    void captureSeeds(int last_hole, int player) {
        /**
        Gère la capture des graines
        Règles :
        - La capture commence UNIQUEMENT si le dernier trou semé est dans les trous adverses
        - Remonte en arrière en sens anti-horaire, uniquement dans les trous de l'adversaire
        - Capture les trous avec exactement 2 ou 3 graines (toutes couleurs confondues)
        - S'arrête dès qu'on rencontre un trou qui n'a pas 2-3 graines
        */
        int opponent = 3 - player;
        std::vector<int> opponent_holes = state->getPlayerHoles(opponent);

        // La capture ne s'effectue que si le dernier trou semé est dans les trous adverses
        if (std::find(opponent_holes.begin(), opponent_holes.end(), last_hole) == opponent_holes.end()) {
            if (debug) {
                std::cout << "[DEBUG] Pas de capture - le dernier trou semé " << last_hole 
                          << " n'est pas un trou adverse" << std::endl;
            }
            return;
        }

        if (debug) {
            std::cout << "[DEBUG] Vérification de capture en partant du trou " << last_hole << std::endl;
        }

        int current_hole = last_hole;
        int captured_total = 0;

        // Remonte en arrière en sens anti-horaire, uniquement dans les trous de l'adversaire
        while (std::find(opponent_holes.begin(), opponent_holes.end(), current_hole) != opponent_holes.end()) {
            int total_seeds = state->getTotalSeeds(current_hole);

            if (debug) {
                std::cout << "[DEBUG] Trou " << current_hole << ": " << total_seeds << " graines" << std::endl;
            }

            if (total_seeds == 2 || total_seeds == 3) {
                // Capture ce trou
                int seeds_captured = total_seeds;
                captured_total += seeds_captured;

                if (debug) {
                    std::cout << "[DEBUG] ✓ Capture du trou " << current_hole 
                              << " (" << seeds_captured << " graines)" << std::endl;
                }

                // Réinitialise le trou
                for (Color col : {Color::RED, Color::BLUE, Color::TRANSPARENT}) {
                    state->holes[current_hole][col] = 0;
                }

                state->captured_seeds[player] += seeds_captured;

                // Continue vers le trou précédent
                current_hole = prevHole(current_hole);
            } else {
                // Arrête la capture
                if (debug) {
                    std::cout << "[DEBUG] ✗ Fin de capture - le trou " << current_hole 
                              << " n'a pas 2-3 graines (" << total_seeds << ")" << std::endl;
                }
                break;
            }
        }

        if (debug && captured_total > 0) {
            std::cout << "[DEBUG] Total capturé: " << captured_total << " graines" << std::endl;
        }
    }

public:
    std::string getBoardStateString() const {
        /**Retourne une représentation lisible du plateau*/
        return state->toString();
    }
};


class MoveGenerator {
public:
    /**Générateur de mouvements pour l'IA*/

    static std::vector<std::tuple<int, Color, Color, bool>> getAllMoves(const GameState& state, int player) {
        /**
        Génère tous les coups possibles pour un joueur
        IMPORTANT: Les graines TRANSPARENTES ne sont JAMAIS générées automatiquement.
        Elles ne peuvent être jouées que si explicitement demandées (format "5TR" ou "5TB")
        par le joueur humain.

        Pour l'IA et les mouvements automatiques: UNIQUEMENT les couleurs RED et BLUE

        Retourne: (hole, color_to_play, transparent_as_color, use_transparent)
        */
        std::vector<std::tuple<int, Color, Color, bool>> moves;

        for (int hole : state.getPlayerHoles(player)) {
            // Générer les coups UNIQUEMENT pour les couleurs ROUGE et BLEU
            // Les graines TRANSPARENTES ne sont PAS incluses ici
            for (Color color : {Color::RED, Color::BLUE}) {
                if (state.holes.at(hole).at(color) > 0) {
                    moves.push_back({hole, color, Color::RED, false});
                }
            }
        }

        return moves;
    }

    static GameState applyMove(const GameState& state, int hole, Color color, Color transparent_as = Color::RED, bool use_transparent = false) {
        /**
        Applique un coup à un état et retourne le nouvel état
        */
        GameState new_state = state.copy();
        GameEngine engine(&new_state);
        engine.playMove(hole, color, transparent_as, use_transparent);
        return new_state;
    }
};

#endif // GAME_ENGINE_H
