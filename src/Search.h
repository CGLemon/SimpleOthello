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
					int alpha = std::numeric_limits<int>::min(),
					int beta  = std::numeric_limits<int>::max()); 

	int get_best_move();

	void print_move_eval(bool quiet) const;
	void print_node_count(bool quiet) const;
	void parser_vertex(const int vtx, bool quiet) const;

	void set_time(int seconds);
	void set_maxdeep(int maxdeep);
private:
	GameState & root_state;
	std::unique_ptr<Node> root_node;
	int m_maxdeep{4};
	int m_searchdeep;
	int m_search_seconds{5};
	bool m_running;
	Timer m_TimeController;
};

#endif
