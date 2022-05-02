#include <sstream>
#include "uci.h"

void UCIInterface::UCILoop() {
    bootEngine();
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        if (command == "uci") {
            std::cout << "id name " << "Horowitz" << std::endl;
            std::cout << "id author " << "OliveriQ" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (command == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (command == "ucinewgame") {
            pos.parseFEN(defaultFEN);
        }
        else if (command == "position") {
            std::string subcommand;
            iss >> subcommand;
            if (subcommand == "startpos") {
                pos.parseFEN(defaultFEN);
            }
            else if (subcommand == "fen") {
                std::string temp;
                std::string fen="";
                for (int i = 0; i < 6; i++) {
                    iss >> temp;
                    fen += " " + temp;
                }
                pos.parseFEN(fen);
            }
            iss >> subcommand;
            if (subcommand == "moves") {
                std::string moveUci;
                while (iss >> moveUci) {
                    Move move = parseMove(moveUci);
                    if (pos.sideToMove == White) 
                        pos.makemove<White>(move);
                    else pos.makemove<Black>(move);
                }
            }
        }
        else if (command == "go") {
            if (pos.sideToMove == White) 
                search.search<White>(pos, maxPly + 1);
            else
                search.search<Black>(pos, maxPly + 1);

        }
        else if (command == "quit") {
            break;
        }
        else if (command == "print") {
            pos.print();
        }
    }
}

Move UCIInterface::parseMove(std::string moveUci) {
    Moves moveList = (pos.sideToMove == White) ? 
        pos.generateLegalMoves<White>() : pos.generateLegalMoves<Black>();

    Square source = Square((moveUci[1] - 49) * 8 + (moveUci[0] - 97));
    Square target = Square((moveUci[3] - 49) * 8 + (moveUci[2] - 97));
    PieceType promoted = Nonetype;

    if (moveUci.size() == 5) {
        promoted = (moveUci[4] == 'q') ? Queen :
            (moveUci[4] == 'r') ? Rook :
            (moveUci[4] == 'b') ? Bishop :
            (moveUci[4] == 'n') ? Knight :
            Nonetype;
    }

    for (int i = 0; i < moveList.count; i++) {
        Move move = moveList.moves[i];
        if (move.source() == source && move.target() == target) {
            if (promoted == Nonetype && move.promoted() == 0) {
                return move;
            }
            else if (move.promoted() > 0 && move.piece() == promoted) {
                return move;
            } 
        }
    }
    return Move();
}

void UCIInterface::bootEngine() {
    pos = Position();
    
    std::cout << "Horowitz v2.0: UCI Compatible chess engine\n";
}