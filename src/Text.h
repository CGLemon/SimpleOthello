#ifndef TEXT_H_INCLUDE
#define TEXT_H_INCLUDE
#include "GameState.h"

#include <string>
#include <vector>


class Text{
public:
	static void init_all();
	static void text(std::string &, GameState & state, bool quite = false);
	static std::vector<std::string> first_cmd; 
};




#endif
