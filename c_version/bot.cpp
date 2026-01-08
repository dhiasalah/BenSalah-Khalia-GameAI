/**
 * Bot pour la plateforme awale-arena - Version C++
 * Format compatible:
 * - Entrée: mouvements au format "NX" ou "NTX" (N=numéro trou, X=R/B/T, T=transparent)
 * - Sortie: coup au même format
 * - Utilise MinMax AI de ai_algorithms.h
 */

#include "game_rules.h"
#include "game_engine.h"
#include "ai_algorithms.h"
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <optional>

// Structure pour représenter un coup parsé
struct ParsedMove
{
    int hole;
    Color color;
    std::optional<Color> trans_as;
    bool valid;
};

// Parse un mouvement au format "NX" ou "NTX"
ParsedMove parse_move(const std::string &move_str)
{
    ParsedMove result = {0, Color::RED, std::nullopt, false};

    try
    {
        std::string upper_move = move_str;
        for (auto &c : upper_move)
            c = std::toupper(c);

        // Chercher 'T' au milieu (transparent)
        if (upper_move.length() >= 3 && upper_move[upper_move.length() - 2] == 'T')
        {
            // Format "NTX" (ex: "3TR", "3TB")
            result.hole = std::stoi(upper_move.substr(0, upper_move.length() - 2));
            result.color = Color::TRANSPARENT;

            char trans_as_char = upper_move[upper_move.length() - 1];
            result.trans_as = (trans_as_char == 'R') ? Color::RED : Color::BLUE;
            result.valid = true;
        }
        else if (upper_move.length() >= 2)
        {
            // Format "NX" (ex: "3R", "4B")
            result.hole = std::stoi(upper_move.substr(0, upper_move.length() - 1));

            char color_char = upper_move[upper_move.length() - 1];
            result.color = (color_char == 'R') ? Color::RED : Color::BLUE;
            result.valid = true;
        }
    }
    catch (...)
    {
        result.valid = false;
    }

    return result;
}

// Formate un mouvement pour l'envoi
std::string format_move(int hole, Color color, std::optional<Color> trans_as)
{
    std::stringstream ss;
    ss << hole;

    if (color == Color::TRANSPARENT && trans_as.has_value())
    {
        ss << "T" << (trans_as.value() == Color::RED ? "R" : "B");
    }
    else
    {
        ss << (color == Color::RED ? "R" : "B");
    }

    return ss.str();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: bot <player_number>" << std::endl;
        return 1;
    }

    // Récupérer le numéro du joueur (1 ou 2)
    int my_player = std::stoi(argv[1]);

    // Initialiser le jeu et le bot avec MinMax AI
    GameState state;
    GameEngine engine(&state); // Pass pointer to GameState
    MinMaxBot bot(4);          // MinMax avec profondeur 4

    std::string line;

    // Boucle principale
    while (std::getline(std::cin, line))
    {
        // Enlever les espaces blancs
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        // Traiter les signaux spéciaux
        if (line == "END")
        {
            break;
        }

        // Pour Player 2, on doit d'abord recevoir le coup de Player 1
        if (line == "START" && my_player == 2)
        {
            continue; // Attendre le prochain message (le coup de Player 1)
        }

        if (line != "START")
        {
            // Appliquer le coup de l'adversaire
            ParsedMove parsed = parse_move(line);
            if (parsed.valid)
            {
                if (parsed.color == Color::TRANSPARENT && parsed.trans_as.has_value())
                {
                    engine.playMove(parsed.hole, parsed.color, parsed.trans_as.value(), true);
                }
                else
                {
                    engine.playMove(parsed.hole, parsed.color, Color::RED, false);
                }
            }
        }

        // S'assurer que le joueur courant est correct
        state.current_player = my_player;

        // Obtenir le meilleur coup avec MinMax
        Move best_move = bot.getMove(state, my_player);

        std::string my_move;

        if (best_move.valid)
        {
            std::optional<Color> trans_opt = best_move.use_transparent ? std::optional<Color>(best_move.transparent_as) : std::nullopt;
            my_move = format_move(best_move.hole, best_move.color, trans_opt);

            // Appliquer notre coup
            engine.playMove(best_move.hole, best_move.color,
                            best_move.transparent_as, best_move.use_transparent);
        }
        else
        {
            // Pas de coup disponible (ne devrait pas arriver)
            my_move = "NOMOVE";
        }

        // Envoyer le coup à la plateforme
        std::cout << my_move << std::endl;
        std::cout.flush();
    }

    return 0;
}
