#ifndef SEARCH_H_INCLUDE
#define SEARCH_H_INCLUDE

#include <vector>
#include <memory>
#include <mutex>
#include <queue>
#include <atomic>

#include "Board.h"
#include "GameState.h"
#include "Node.h"
#include "Evaluation.h"
#include "Timer.h"
class SearchList {
public:

	SearchList() = default;
	SearchList(
		Board set_board,
		std::vector<int> set_movelist,
		Node * set_node,
		int set_alpha,
		int set_beta,
		int set_searchdepth
	) : 
		searchdepth(set_searchdepth),
		alpha(set_alpha),
		beta(set_beta),
		node(set_node),
		movelist(set_movelist),
		current_board(set_board)
	{}

	int searchdepth;
	int alpha;
	int beta;
	Node * node;
	std::vector<int> movelist;
	Board current_board;
};

class UpdateList {
public:
	UpdateList(Node * set_node, int set_color) : 
					node(set_node), color(set_color) {}
	int color;
	Node * node;
};
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

	void print_move_eval(bool quiet) const;
	void print_node_count(bool quiet) const;
	void parser_vertex(const int vtx, bool quiet) const;

	void set_time(int seconds);
	void set_maxdepth(int maxdepth);

	static void thread_seach_0(Search * search);
	static void thread_seach(Search * search);
private:
	GameState & root_state;
	std::unique_ptr<Node> root_node;
	int m_maxdepth{99};
	int m_basicdepth{4};
	int m_searchdepth;
	int m_search_seconds{5};

	Timer m_TimeController;
	
	std::mutex m_mutex;
	SearchList m_list;
	std::queue<SearchList> search_queue;
	std::queue<UpdateList> update_queue;
	
	std::atomic<size_t> m_running;
	bool m_searching;
	int m_cpus;
};

#endif
