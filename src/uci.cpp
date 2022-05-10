#include "uci.h"
#include "zobrist.h"
#include "tt.h"

// main UCI loop
void UCIInterface::UCILoop() {
    bootEngine();
    std::string line;
    while (std::getline(std::cin, line)) {
        iss = std::istringstream(line);
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
            search.TT.Clear();
            search.pos = Position();
        }
        else if (command == "position") {
            std::string subcommand;
            iss >> subcommand;
            if (subcommand == "startpos") {
                search.pos.parseFEN(defaultFEN);
            }
            else if (subcommand == "fen") {
                std::string temp;
                std::string fen="";
                for (int i = 0; i < 6; i++) {
                    iss >> temp;
                    fen += " " + temp;
                }
                search.pos.parseFEN(fen);
            }
            iss >> subcommand;
            if (subcommand == "moves") {
                std::string moveUci;
                while (iss >> moveUci) {
                    Move move = parseMove(moveUci);
                    if (search.pos.sideToMove == White) 
                        search.pos.makemove<White>(move);
                    else search.pos.makemove<Black>(move);
                }
            }
        }
        else if (command == "count") {
            std::cout << search.pos.storeCount << std::endl;
        }
        else if (command == "go") {
            stopThread();
            parseGoCommand();
        }
        else if (command == "quit") {
            stopThread();
            break;
        }
        else if (command == "stop") {
            stopThread();
        }
        else if (command == "print") {
            search.pos.print();
        }
        else if (command == "moves") {
            Moves moveList = search.pos.generateLegalMoves<White>();
            for (int i = 0; i < moveList.count; i++) 
                std::cout << moveList.moves[i].toUci() << std::endl;
        }
    }
}

Move UCIInterface::parseMove(std::string moveUci) {
    Moves moveList = (search.pos.sideToMove == White) ? 
        search.pos.generateLegalMoves<White>() : search.pos.generateLegalMoves<Black>();

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

void UCIInterface::parseGoCommand() {
    std::string line;
    std::string command;

    int timeLeft   = InfiniteTime;
    int increment  = NoValue;
    int movesToGo  = NoValue;
    int searchTime = NoValue;
    int depth      = maxPly;

    char colorPrefix = (search.pos.sideToMove == White) ? 'w' : 'b';

    for (int i = 0; i < 20; i++) {
        iss >> command;
        if (command == "wtime"      && colorPrefix == 'w') {
            iss >> timeLeft;
        }
        else if (command == "btime" && colorPrefix == 'b') {
            iss >> timeLeft;
        }
        else if (command == "winc"  && colorPrefix == 'w') {
            iss >> increment;
        }
        else if (command == "binc"  && colorPrefix == 'b') {
            iss >> increment;
        }
        else if (command == "movestogo") {
            iss >> movesToGo;
        }
        else if (command == "depth") {
            iss >> depth;
        }
        else if (command == "movetime") {
            iss >> timeLeft;
        }
    }

    // Set the time_left to NoValue if we're already given a move time
    // to use via movetime.
    if (searchTime != NoValue) 
        timeLeft = NoValue;

    // Setup the timer with the go command time control information.
    search.timer.setHardTimeForMove(searchTime);
    search.timer.TimeLeft = timeLeft;
    search.timer.Increment = increment;
    search.timer.MovesToGo = movesToGo;

    beginThread(depth);
}

void UCIInterface::bootEngine() {
    zobrist.initRandomKeys();
    search.TT.Init(64);
    search.pos = Position(defaultFEN);
    search.timer.Stop = false;
    
    std::cout << "Horowitz v2.0: UCI-Compatible chess engine\n";
}

void UCIInterface::beginThread(int depth) {
    if (isSearching()) 
        stopThread();
    search.timer.Stop = false;
    threads = search.pos.sideToMove == White ?
            std::thread(&Search::search<White>, std::ref(search), depth) :
            std::thread(&Search::search<Black>, std::ref(search), depth);
}

void UCIInterface::stopThread() {
    search.timer.Stop = true;
    if (threads.joinable()) 
        threads.join();
}

bool UCIInterface::isSearching() {
    return threads.joinable();
}