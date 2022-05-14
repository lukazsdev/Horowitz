#pragma once

#include <string>
#include <unordered_map>

// This is Horowitz's internally hardcoded opening book.
// The opening book uses a hash map to map the current hash
// key of the position to the best move. So the time complexity
// is O(1) on average. 
static std::unordered_map<uint64_t, std::string> OpeningBook({
    {16189388441623395134ULL, "e2e4"},
    {15140732797413565673ULL, "e7e5"},
    {13254008548937051894ULL, "g1f3"},
    {18325115906745120207ULL, "b8c6"},
    {8570469182106178584ULL,  "f1c4"},
    {8020162699011262252ULL,  "f8c5"},
    {2158084884857562495ULL,  "c2c3"},
    {784881719803473815ULL,   "g8f6"},
    {9186543778304483003ULL, "d7d5"},
    {9186543778304483003ULL, "d7d5"},
    {2802111869825210918ULL, "e7e6"},
    {7455774412413129671ULL, "g1f3"}
});

/*
uci.search.pos.parseFEN("fen");
uci.search.pos.generateHashKey();
std::cout << "{" << uci.search.pos.hashKey << "ULL" << ", \"move\"},\n";
*/