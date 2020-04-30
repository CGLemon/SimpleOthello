#ifndef GAMESTATE_H_INCLUDE
#define GAMESTATE_H_INCLUDE
#include "Board.h"
#include "config.h"

#include <vector>
#include <memory>

class GameState {
public:
	GameState();
	void init_game(int boardsize);
	void display() const;	
	
	bool play_move(const int color, const int vtx);
	bool text_play_move(std::string &input);
	bool is_gameover() const;
	int  get_winner() const;

	Board board;
private:
	std::vector<std::shared_ptr<const Board>> game_history;
	int m_winner;
};



#endif
