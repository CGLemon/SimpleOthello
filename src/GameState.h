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
	
	bool undo_move();
	bool is_gameover() const;
	int  get_winner(bool ret = false);
	
	void one_resign(const int resign_color);
	void force_countine();
	int get_movenum() const;

	
	std::shared_ptr<const Board> get_board(const int v); 
	Board board;

private:
	std::vector<std::shared_ptr<const Board>> game_history;
	int m_movenum;
	
	bool m_gameover;
	int m_winner;	
};



#endif
