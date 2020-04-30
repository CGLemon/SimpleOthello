#ifndef SEARCH_H_INCLUDE
#define SEARCH_H_INCLUDE

#include <vector>
#include <memory>

#include "Board.h"
#include "GameState.h"
#include "Node.h"
#include "Evaluation.h"
#include "Timer.h"


class Search {
public:
	using MoveList = std::vector<int>;

	Search(GameState & state) : root_state(state) {}
	void clear();
	bool prepare_root();
	int think();
	void search(const Board & current_board, Node * node, 
					MoveList movelist, int maxdeep,
					int alpha, int beta); 

	int get_best_move();

	void print_move_eval(bool quiet) const;
	void print_node_count(bool quiet) const;

private:
	GameState & root_state;
	std::unique_ptr<Node> root_node;
	int m_maxdeep{8};
	int m_searchdeep;
	Timer m_TimeController;

};

#endif
