#include "game_rules.h"
#include "game_engine.h"
#include "ai_algorithms.h"
#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <optional>
#include <chrono>

struct ParsedMove
{
    int hole;
    Color color;
    std::optional<Color> trans_as;
    bool valid;
};

ParsedMove parse_move(const std::string &move_str)
{
    ParsedMove result = {0, Color::RED, std::nullopt, false};

    try
    {
        std::string upper_move = move_str;
        for (auto &c : upper_move)
            c = std::toupper(c);

        if (upper_move.length() >= 3 && upper_move[upper_move.length() - 2] == 'T')
        {
            result.hole = std::stoi(upper_move.substr(0, upper_move.length() - 2));
            result.color = Color::TRANSPARENT;

            char trans_as_char = upper_move[upper_move.length() - 1];
            result.trans_as = (trans_as_char == 'R') ? Color::RED : Color::BLUE;
            result.valid = true;
        }
        else if (upper_move.length() >= 2)
        {
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

    int my_player = (std::string(argv[1]) == "JoueurA") ? 1 : 2;

    GameState state;
    GameEngine engine(&state);
    MinMaxBot bot(1);
    int move_count = 0;
    std::string last_move;

    std::string line;

    auto check_game_over = [&]() -> std::string
    {
        int score_j1 = state.captured_seeds[1];
        int score_j2 = state.captured_seeds[2];

        if (score_j1 >= 49 || score_j2 >= 49)
        {
            return "RESULT " + last_move + " " + std::to_string(score_j1) + " " + std::to_string(score_j2);
        }

        if (move_count >= 400)
        {
            return "RESULT LIMIT " + std::to_string(score_j1) + " " + std::to_string(score_j2);
        }

        int total_on_board = 0;
        for (int i = 1; i <= 16; i++)
        {
            total_on_board += state.holes[i][Color::RED] + state.holes[i][Color::BLUE] + state.holes[i][Color::TRANSPARENT];
        }
        if (total_on_board < 10)
        {
            return "RESULT " + last_move + " " + std::to_string(score_j1) + " " + std::to_string(score_j2);
        }

        return "";
    };

    while (std::getline(std::cin, line))
    {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line == "START" && my_player == 2)
        {
            continue;
        }

        if (line != "START")
        {
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
                move_count++;
                last_move = line;

                std::string result = check_game_over();
                if (!result.empty())
                {
                    std::cout << result << std::endl;
                    break;
                }
            }
        }

        state.current_player = my_player;

        auto start_time = std::chrono::steady_clock::now();

        Move best_move = bot.findBestMove(state, my_player, std::chrono::milliseconds(2000));

        auto end_time = std::chrono::steady_clock::now();
        long total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        std::cerr << "[C++] Move time: " << total_time << "ms" << std::endl;

        std::string my_move;

        if (best_move.valid)
        {
            std::optional<Color> trans_opt = best_move.use_transparent ? std::optional<Color>(best_move.transparent_as) : std::nullopt;
            my_move = format_move(best_move.hole, best_move.color, trans_opt);

            engine.playMove(best_move.hole, best_move.color,
                            best_move.transparent_as, best_move.use_transparent);
            move_count++;
            last_move = my_move;

            std::cout << my_move << std::endl;
            std::cout.flush();

            std::string result = check_game_over();
            if (!result.empty())
            {
                std::cout << result << std::endl;
                break;
            }
        }
        else
        {
            int score_j1 = state.captured_seeds[1];
            int score_j2 = state.captured_seeds[2];
            std::cout << "RESULT " << last_move << " " << score_j1 << " " << score_j2 << std::endl;
            break;
        }
    }

    return 0;
}
