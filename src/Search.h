#ifndef SEARCH_H_INCLUDE
#define SEARCH_H_INCLUDE

#include <vector>
#include <memory>

#include "Utils.h"
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
					MoveList movelist, int maxdepth,
					int alpha = std::numeric_limits<int>::min(),
					int beta  = std::numeric_limits<int>::max()); 

	int get_best_move();

	void print_move_eval() const;
	void print_node_count() const;
	void parser_vertex(const int vtx) const;

	void set_time(int seconds);
	void set_maxdepth(int maxdepth);
	void set_basicdepth(int basicdepth);
private:
	GameState & root_state;
	std::unique_ptr<Node> root_node;
	int m_maxdepth{99};
	int m_basicdepth{8};
	int m_searchdepth;
	int m_search_seconds{5};
	bool m_running;
	Timer m_TimeController;
};

#endif
