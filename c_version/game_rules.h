/**
 * Règles du jeu Mancala à 16 trous
 * - 16 trous (8 par joueur)
 * - Numérotés de 1 à 16, en sens horaire
 * - Joueur 1: trous impairs (1,3,5,7,9,11,13,15)
 * - Joueur 2: trous pairs (2,4,6,8,10,12,14,16)
 * - Au départ: 2 graines rouges, 2 bleues, 2 transparentes par trou
 * - Trois couleurs: Red (R), Blue (B), Transparent (T)
 */

#ifndef GAME_RULES_H
#define GAME_RULES_H

#include <map>
#include <vector>
#include <string>
#include <sstream>

enum class Color
{
    RED,
    BLUE,
    TRANSPARENT
};

inline std::string colorToString(Color c)
{
    switch (c)
    {
    case Color::RED:
        return "R";
    case Color::BLUE:
        return "B";
    case Color::TRANSPARENT:
        return "T";
    }
    return "";
}

// Pre-computed player holes for speed
const int PLAYER1_HOLES[8] = {1, 3, 5, 7, 9, 11, 13, 15};
const int PLAYER2_HOLES[8] = {2, 4, 6, 8, 10, 12, 14, 16};

class GameState
{
public:
    static const int MAX_MOVES = 400; // Limite de 400 coups (200 par joueur)

    // Dictionnaire des trous: {numéro: {couleur: nombre}}
    std::map<int, std::map<Color, int>> holes;
    std::map<int, int> captured_seeds; // Graines capturées par joueur
    int current_player;
    int move_count; // Compteur de coups joués

    GameState() : current_player(1), move_count(0)
    {
        captured_seeds[1] = 0;
        captured_seeds[2] = 0;
        initializeBoard();
    }

    void initializeBoard()
    {
        /**Initialise le plateau avec 2 graines de chaque couleur par trou*/
        for (int hole = 1; hole <= 16; hole++)
        {
            holes[hole][Color::RED] = 2;
            holes[hole][Color::BLUE] = 2;
            holes[hole][Color::TRANSPARENT] = 2;
        }
    }

    const int *getPlayerHolesPtr(int player) const
    {
        /**Retourne pointeur vers les trous du joueur (pour performance)*/
        return (player == 1) ? PLAYER1_HOLES : PLAYER2_HOLES;
    }

    std::vector<int> getPlayerHoles(int player) const
    {
        /**Retourne les trous contrôlés par un joueur
        Joueur 1: trous impairs, Joueur 2: trous pairs*/
        if (player == 1)
        {
            return std::vector<int>(PLAYER1_HOLES, PLAYER1_HOLES + 8);
        }
        else
        {
            return std::vector<int>(PLAYER2_HOLES, PLAYER2_HOLES + 8);
        }
    }

    int getTotalSeeds(int hole) const
    {
        /**Retourne le nombre total de graines dans un trou - Optimized*/
        const auto &h = holes.at(hole);
        return h.at(Color::RED) + h.at(Color::BLUE) + h.at(Color::TRANSPARENT);
    }

    int getSeedsOnBoard() const
    {
        /**Retourne le nombre total de graines sur le plateau*/
        int total = 0;
        for (const auto &hole : holes)
        {
            for (const auto &color : hole.second)
            {
                total += color.second;
            }
        }
        return total;
    }

    bool isGameOver() const
    {
        /**
        Vérifie si le jeu est terminé selon les règles - Optimized
        */
        // Condition 0: Limite de 400 coups atteinte
        if (move_count >= 400)
        {
            return true;
        }

        int c1 = captured_seeds.at(1);
        int c2 = captured_seeds.at(2);

        // Condition 2: Un joueur a capturé 49+ graines (victoire)
        if (c1 >= 49 || c2 >= 49)
        {
            return true;
        }

        // Condition 3: Les deux joueurs ont capturé 40+ graines (égalité)
        if (c1 >= 40 && c2 >= 40)
        {
            return true;
        }

        // Condition 1: Moins de 10 graines sur le plateau (fast calc)
        if (96 - c1 - c2 < 10)
        {
            return true;
        }

        return false;
    }

    int getWinner() const
    {
        /**
        Retourne le gagnant: 1, 2 ou 0 (égalité)
        Règles:
        - Joueur avec 49+ graines gagne
        - Si les deux ont 40+: égalité
        - Si moins de 10 graines: celui avec le plus de graines gagne
        - Sinon: celui avec le plus de graines gagne
        */
        // Si un joueur a 49+, il gagne
        if (captured_seeds.at(1) >= 49)
        {
            return 1;
        }
        if (captured_seeds.at(2) >= 49)
        {
            return 2;
        }

        // Sinon, compare les scores
        if (captured_seeds.at(1) > captured_seeds.at(2))
        {
            return 1;
        }
        else if (captured_seeds.at(2) > captured_seeds.at(1))
        {
            return 2;
        }
        else
        {
            return 0; // Égalité
        }
    }

    std::vector<std::pair<int, Color>> getValidMoves(int player) const
    {
        /**Retourne les coups valides pour un joueur
        Format: (numéro_trou, couleur)*/
        std::vector<std::pair<int, Color>> valid_moves;
        std::vector<int> player_holes = getPlayerHoles(player);

        for (int hole : player_holes)
        {
            for (Color color : {Color::RED, Color::BLUE, Color::TRANSPARENT})
            {
                if (holes.at(hole).at(color) > 0)
                {
                    valid_moves.push_back({hole, color});
                }
            }
        }

        return valid_moves;
    }

    GameState copy() const
    {
        /**Crée une copie de l'état du jeu*/
        GameState new_state;
        for (int hole = 1; hole <= 16; hole++)
        {
            new_state.holes[hole] = holes.at(hole);
        }
        new_state.captured_seeds = captured_seeds;
        new_state.current_player = current_player;
        new_state.move_count = move_count;
        return new_state;
    }

    std::string toString() const
    {
        /**Affichage du plateau*/
        std::stringstream result;
        result << "\n"
               << std::string(80, '=') << "\n";
        result << "Player 1 captured: " << captured_seeds.at(1) << " seeds\n";
        result << "Player 2 captured: " << captured_seeds.at(2) << " seeds\n";
        result << "Current player: " << current_player << "\n";
        result << std::string(80, '=') << "\n";

        // Affichage du plateau
        result << "Holes 16-15-14-13-12-11-10-9\n";
        for (int h = 16; h >= 9; h--)
        {
            result << h << "(" << getTotalSeeds(h) << ") ";
        }
        result << "\n";

        for (int h = 16; h >= 9; h--)
        {
            result << "R:" << holes.at(h).at(Color::RED)
                   << " B:" << holes.at(h).at(Color::BLUE)
                   << " T:" << holes.at(h).at(Color::TRANSPARENT) << "    ";
        }
        result << "\n";

        result << "\nHoles 1-2-3-4-5-6-7-8\n";
        for (int h = 1; h <= 8; h++)
        {
            result << h << "(" << getTotalSeeds(h) << ") ";
        }
        result << "\n";

        for (int h = 1; h <= 8; h++)
        {
            result << "R:" << holes.at(h).at(Color::RED)
                   << " B:" << holes.at(h).at(Color::BLUE)
                   << " T:" << holes.at(h).at(Color::TRANSPARENT) << "    ";
        }
        result << "\n";

        return result.str();
    }
};

#endif // GAME_RULES_H
