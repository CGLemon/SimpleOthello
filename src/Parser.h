#ifndef PARSER_H_INCLUDE
#define PARSER_H_INCLUDE

#include "GameState.h"
#include <string>


class Parser {
	
/*

*/

public:
	static void simple_parser(GameState & state, std::string & thor);
	static bool load_thor(GameState & state, std::string filename);

	static std::string simple_thor(GameState & state, bool withpass = false);
	static bool save_thor(GameState & state, std::string filename, bool withpass = false);

	static std::string parser_vertex(const int vtx, const int boardsize, bool character = false);

private:


};

#endif
